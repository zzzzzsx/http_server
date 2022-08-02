
#ifndef CPP_HTML_PARSE
#define CPP_HTML_PARSE
#include <algorithm>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

// enum HttpMethod {
//     GET = 0,
//     POST
// };
// enum HttpStatus {
//     GET = 0,
//     POST
// };

class HtmlParse {
private:
    std::unordered_map<std::string, std::string> header;
    std::string body;
    bool illegalRequest = false;
    std::string response;


public:
    HtmlParse();
    ~HtmlParse();
    std::pair<bool, std::unordered_map<std::string, std::string>> getHeader();
    std::pair<bool, std::string> getBody();
    std::string getResponse(const std::string &);
    void analyzeRequest();
    inline std::vector<std::string> split(const std::string &, const std::string &);
};

#endif