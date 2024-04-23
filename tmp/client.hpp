#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <map>

typedef std::map<std::string, std::string> Headers;

enum {
    READ_NOT_DONE = 0,
    READ_DONE = 1,
    READ_ERROR = 2
};

struct Request {
    std::string _method;
    std::string _uri;
    std::string _version;
    Headers      _headers;
    // std::map<std::string, std::string> cookie;
    // std::string _body; //set env 할거라 보관할 필요 x, 혹시 몰라 그냥 라인째로 넣어만 둠.
    int         _readStatus;
    // int         _status; //아직 쓸진 모르겠지만 response를 위해 일단 넣어둠.
};

//todo
/*
	1. 쿠키 파싱 (쿠키의 어떤 데이터를 어디서 어떻게 사용할지에 따라서
					(map으로 해서 들고다닐지 고민해야함
*/

class Client {
    private:
        std::string _buffer;
        Request     _request;
        int         _status;
    public:
        std::string getBuffer() const;
        std::string getMethod() const;
        std::string getUri() const;
        std::string getVersion() const;
        std::string getHeader(const std::string& key) const;
        // std::string getBody() const;

        void setRequest(const Request& request);
        void clearRequest();

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

// std::string Client::getBody() const {
//     return _request._body;
// }

void Client::setRequest(const Request& request) {
    _request = request;
}

void Client::clearRequest()
{
    _request._method.clear();
    _request._uri.clear();
    _request._version.clear();
    _request._headers.clear();
    // _request._body.clear();
}

void Client::printAllHeaders() const{
    const Headers& headers = _request._headers;
    for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

#endif