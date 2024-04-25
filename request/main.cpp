// #include "request.hpp"
// #include "client.hpp"
// #include <fstream>


// int main() {
//     std::string request = "GET /index.html HTTP/1.1\n"
//                          "Host: www.example.com\n"
//                          "User-Agent: Mozilla/5.0\n"
//                          "Content-Type: application/x-www-form-urlencoded\n"
//                          "a: a\n"
//                          "Content-Length: 31\n"
//                          "\r\n\r\n"
//                          "name=John&email=john@example.com";

//     // std::ifstream ifs("test");

//     // std::string request((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    

//     // HttpRequest req;
//     Client client;
//     client.setRequest(HttpRequest::parse(request));

//     // std::cout << "Method: " << client.getMethod() << std::endl;
//     // std::cout << "URI: " << client.getUri() << std::endl;
//     // std::cout << "Version: " << client.getVersion() << std::endl;

//     // client.printAllHeaders();

//     // client.clearRequest();

//     std::cout << getenv("name") << std::endl;
//     std::cout << getenv("email") << std::endl;

//     // std::cout  << "======================" << std::endl;

//     // std::cout << "Body: " << client.getBody() << std::endl;
//     return 0;
// }

#include <iostream>
#include "Client.hpp"

int main() {
    Client client;

    // 예시 요청 데이터 설정
    std::string requestData = "GET / HTTP/1.1\r\n"
                              "Host: example.com\r\n"
                              "User-Agent: Mozilla/5.0\r\n"
                              "Accept: text/html\r\n"
                              "Cookie: session_id=12345\r\n"
                              "\r\n"
                              "name=John&email=john@example.com";

    client.setPort(80); // 포트 번호 설정
    // client._buffer = requestData; // 요청 데이터 설정
    client.setBuffer(requestData);

    client.setRequest(); // 요청 데이터 파싱

    // 요청 정보 출력
    std::cout << "Method: " << client.getMethod() << std::endl;
    std::cout << "URI: " << client.getUri() << std::endl;
    std::cout << "Version: " << client.getVersion() << std::endl;
    std::cout << "Headers:" << std::endl;
    client.printAllHeaders();
    std::cout << "Body: " << client.getBody() << std::endl;
    std::cout << "Cookie: " << client.getCookie("session_id") << std::endl;
    std::cout << "Response Status: " << client.getResponseStatus() << std::endl;
    

    return 0;
}