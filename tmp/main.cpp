#include "request.hpp"

int main() {
    std::string request = "GET /index.html HTTP/1.1\n"
                         "Host: www.example.com\n"
                         "User-Agent: Mozilla/5.0\n"
                         "Content-Type: application/x-www-form-urlencoded\n"
                         "a: a\n"
                         "\r\n"
                         "name=John&email=john@example.com";

    HttpRequest req;
    req.parse(request);

    std::cout << "Method: " << req.getMethod() << std::endl;
    std::cout << "URI: " << req.getUri() << std::endl;
    std::cout << "Version: " << req.getVersion() << std::endl;

    const std::map<std::string, std::string>& headers = req.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << "Body: " << req.getBody() << std::endl;

    return 0;
}