#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <map>
# include <sstream>
# include "structRq.hpp"
# include "Request.hpp"
# include "../utils/utils.hpp"
# include "../Parse/Config.hpp"

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
        Port         _port;
        int         _readStatus;
        int         _responseStatus;
        std::string _tempResult;
        Client();
    public:
        Client(Port port);
        Client(const Client &Copy);
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
        void            setRequest();
        void            setBuffer(const std::string& buffer);
        void            clearRequest();
        void            clearAll();
        int             getReadStatus() const {return this->_readStatus;}

        void            execute(const Config &Conf);

        //getReadStatus;

        void printAllHeaders() const;
        std::string getTempResult() const {
            return this->_tempResult;
        }
};

#endif