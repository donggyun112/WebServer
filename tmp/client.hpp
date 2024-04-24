#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <map>

typedef std::map<std::string, std::string> Headers;
typedef std::map<std::string, std::string> Cookies;

enum {
    READ_NOT_DONE = 0,
    READ_LINE_DONE = 1,
    READ_HEADER_DONE = 2,
    READ_BODY_DOING = 3,
    READ_DONE = 4
};

struct Request {
    std::string _method;
    std::string _uri;
    std::string _version;
    Headers     _headers;
    Cookies     _cookie;
    std::string _body;
    ini         _contentLength;
    int         _readStatus;
    int         _port;
    // int         _status; //아직 쓸진 모르겠지만 response를 위해 일단 넣어둠.
};

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
        int         getPort() const;
        std::string getHeader(const std::string& key) const;
        std::string getBody() const;
        std::string getCookie(const std::string& key) const;


        void setPort(int port);
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

int Client::getPort() const {
    return _request._port;
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

void Client::setPort(int port) {
    _request._port = port;
}

//recv 체크해서 _status 관리

void Client::setRequest() {
    std::istringstream iss(_buffer);
	std::string line;
	std::string header;
	std::string body;

	try {

        if (iss.str().find("\r\n") == std::string::npos \
                && _request._readStatus == READ_NOT_DONE) {
    		std::getline(iss, line, "\r\n");
	    	HttpRequest::parseRequestLine(_request, line);
            _request._readStatus = READ_LINE_DONE;
        }

		std::string::size_type pos = iss.str().find("\r\n\r\n");
		if (pos != std::string::npos && _request._readStatus == READ_LINE_DONE)
		{
			header = iss.str().substr(0, pos);
            std::istringstream headerStream(header);
            while (std::getline(headerStream, line)) {
                HttpRequest::parseHeader(_request, line);
            }
            _request._readStatus = READ_HEADER_DONE;
		}

        // if () //헤더와 바디 수정중

		if (req._headers.find("Content-Length") != req._headers.end()) {
			int contentLength = atoi(req._headers["Content-Length"].c_str());
			if (body.empty() || contentLength > body.length()) {
				req._readStatus = READ_NOT_DONE;
			}
			std::istringstream bodyStream(body);
			while (getline(bodyStream, line) && !line.empty()) {
				if (contentLength == 0) {
					req._readStatus = READ_DONE;
					break;
				}
				parseBody(line, contentLength);
			}
			if (contentLength > 0) {
				initRequest(req);
				req._readStatus = READ_NOT_DONE;
				return req;
			}
		}
		isVaildRequest(req);
		req._readStatus = READ_DONE;
	}
	catch (std::invalid_argument& e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
		req._readStatus = READ_ERROR;
	}
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

// mothod, url, quary string, header
// body는 string으로 그냥 줘버려

#endif