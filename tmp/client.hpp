#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <map>

typedef std::map<std::string, std::string> Headers;


// TODO //
 // request clear 만들기 
 // body parse 만들기
 // 파싱된 데이터 유효성 검사 및 bad request exceoption처리 할 것 

// reponse
 //content length 생각할것

struct Request {
    std::string _method;
    std::string _uri;
    std::string _version;
    Headers _headers;
    std::string _body;
};

class Client {
    private:
        std::string _buffer;
        Request _request;
    public:
        std::string getBuffer() const;
        std::string getMethod() const;
        std::string getUri() const;
        std::string getVersion() const;
        std::string getHeader(const std::string& key) const;
        std::string getBody() const;

        void setRequest(const Request& request);

        void printAllHeaders() const;
};

std::string Client::getBuffer() const {
    return _buffer;
}

std::string Client::getMethod() const {
    return _request._method;
}

std::string Client::getUri() const {
    return _request._uri;
}

std::string Client::getVersion() const {
    return _request._version;
}

std::string Client::getHeader(const std::string& key) const {
    const Headers::const_iterator it = _request._headers.find(key);
    if (it == _request._headers.end())
        return "";
    return it->second;
}

std::string Client::getBody() const {
    return _request._body;
}

void Client::setRequest(const Request& request) {
    _request = request;
}

void Client::printAllHeaders() const{
    const Headers& headers = _request._headers;
    for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

#endif