#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include "structRq.hpp"
#include "Request.hpp"

class HttpRequest;

enum {
    READ_NOT_DONE = 0,
    READ_LINE_DONE = 1,
    READ_HEADER_DONE = 2,
    READ_BODY_DOING = 3,
    READ_DONE = 4,
    READ_ERROR = 5
};

class Client {
    private:
        std::string _buffer;
        Request     _request;
        int         _port;
        int         _readStatus;
        int         _responseStatus;
    public:
        Client();
        ~Client();

        std::string     getBuffer() const;
        std::string     getMethod() const;
        std::string     getUri() const;
        std::string     getVersion() const;
        std::string     getHeader(const std::string& key) const;
        std::string     getCookie(const std::string& key) const;
        std::string     getBody() const;
        const Request   &getRequest() const;
        int             getPort() const;
        int             getResponseStatus() const;

        void            setPort(int port);
        void            setRequest();
        void            setBuffer(const std::string& buffer) {_buffer = buffer;}
        void        c   clearRequest();
        void            clearAll();

        void printAllHeaders() const;
};

#endif