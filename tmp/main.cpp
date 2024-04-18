#include "request.hpp"
#include "client.hpp"
#include <fstream>


int main() {
    std::string request = "GET /index.html HTTP/1.1\n"
                         "Host: www.example.com\n"
                         "User-Agent: Mozilla/5.0\n"
                         "Content-Type: application/x-www-form-urlencoded\n"
                         "a: a\n"
                         "\r\n"
                         "name=John&email=john@example.com";

    // std::ifstream ifs("test");

    // std::string request((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    

    // HttpRequest req;
    Client client;
    client.setRequest(HttpRequest::parse(request));

    std::cout << "Method: " << client.getMethod() << std::endl;
    std::cout << "URI: " << client.getUri() << std::endl;
    std::cout << "Version: " << client.getVersion() << std::endl;

    client.printAllHeaders();

    std::cout << "Body: " << client.getBody() << std::endl;

    return 0;
}