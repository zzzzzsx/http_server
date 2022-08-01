#include "HtmlParse.h"

HtmlParse::HtmlParse() {
}

HtmlParse::~HtmlParse() {
}

std::pair<bool, std::unordered_map<std::string, std::string>> HtmlParse::getHeader() {
    if (illegalRequest) {
        return std::make_pair(true, header);
    } else {
        return std::make_pair(false, header);
    }
}
std::pair<bool, std::string> HtmlParse::getBody() {
    if (illegalRequest) {
        return std::make_pair(true, body);
    } else {
        return std::make_pair(false, body);
    }
}

std::string HtmlParse::getResponse(const std::string &data) {

    std::vector<std::string> lines = split(data, "\r\n");
    std::vector<std::string> request = split(lines[0], " ");
    if (request[0] == "GET" || request[0] == "POST") {
        header["method"] = request[0];
    } else {
        illegalRequest = true;
    }
    try {
        header["url"] = request[1];
        header["version"] = split(request[2], "/")[1];
    } catch (const std::exception &e) {
        illegalRequest = true;
    }
    if (!illegalRequest) {
        int i = 1;
        for (; i < lines.size(); ++i) {
            if (lines[i] == "") {
                break;
            } else {
                header[split(lines[i], ":")[0]] = split(lines[i], ":")[1];
            }
        }
        for (; i < lines.size(); ++i) {
            body += (lines[i] + "\r\n");
        }
    }

    analyzeRequest();
    return response;
}

void HtmlParse::analyzeRequest() {
    std::string status;
    std::string body;
    if (illegalRequest) {
        status = "400";
    } else {
        struct stat sbuf;
        if (header["url"] == "/") {
            header["url"] = "/index.html";
        }
        std::string path = "./static" + header["url"];
        int src_fd = open(path.c_str(), O_RDONLY, 0);

        if (stat(path.c_str(), &sbuf) < 0) {
            status = "404";
        } else {
            void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
            char *src_addr = static_cast<char *>(mmapRet);
            body = std::string(src_addr, src_addr + sbuf.st_size);
            status = "200";
            munmap(mmapRet, sbuf.st_size);
            close(src_fd);
        }
    }
    if (status == "400") {
        try {
            response = "HTTP/" + header["version"] + " " + status + " Bad Request\r\n";
        } catch (const std::exception &e) {
            response = "HTTP/1.1 " + status + "Bad Request\r\n";
            response += "Content-Length:0\r\n\r\n";
        }
    } else if (status == "404") {
        response = "HTTP/" + header["version"] + " " + status + " Not Found\r\n";
        response += "Content-Length:0\r\n\r\n";
    } else if (status == "200") {
        response = "HTTP/" + header["version"] + " " + status + " OK\r\n";
        if (header.find("Connection") != header.end()) {
            if (header["Connection"] == "keep-alive") {
                response += "Connection:keep-alive\r\n";
            }
        }
        // response += "Content-Type:image/png\r\n";
        response += "Content-Length:" + std::to_string(body.size()) + "\r\n\r\n";
        response += body;
    }
}

inline std::vector<std::string> HtmlParse::split(const std::string &s, const std::string &delim) {
    bool keep_empty = true;
    using namespace std;
    vector<string> result;
    if (delim.empty()) {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        if (keep_empty || substart != subend)
            result.emplace_back(substart, subend);
        if (subend == s.end())
            break;
        substart = subend + delim.size();
    }
    return result;
}
