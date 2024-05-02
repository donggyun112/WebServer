#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <map>
# include <sstream>
# include <algorithm>
# include <sys/types.h>
# include <sys/stat.h>
# include <cstring>
# include <iomainp>
# include <dirent.h> // DIR을 위함.
# include "structRq.hpp"
# include "Request.hpp"
# include "../utils/utils.hpp"
# include "../Parse/Config.hpp"

class HttpRequest;
class Response;
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
        Port             getPort() const;
        int             getResponseStatus() const;
        void            setRequest();
        void            setBuffer(const std::string& buffer);
        void            clearRequest();
        void            clearAll();
        int             getReadStatus() const {return this->_readStatus;}

        std::string     execute(const Config &Conf);

		// void			setResponseStatus(int status);
		// void			setTempResult(const std::string &result);
		// void 			setHeader(const std::string &key, const std::string &value);
		Response			sendResponse(const Config &Conf);
		Response			handleGetRequest(const Config &Conf);

        //getReadStatus;

        // autoindex 
            //  std::string     handleAutoIndex(std::string servRoot);
        void printAllHeaders() const;
        std::string getTempResult() const {
            return this->_tempResult;
        }
};

#endif