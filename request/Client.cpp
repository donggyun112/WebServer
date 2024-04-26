#include "Client.hpp"

Client::Client() {
    _port = 0;
    clearAll();
}

Client::~Client() {
    clearAll();
}

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
    return _port;
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

std::string Client::getCookie(const std::string& key) const {
    const Cookies::const_iterator it = _request._cookie.find(key);
    if (it == _request._cookie.end())
        return "";
    return it->second;
}

const Request &Client::getRequest() const {
    return _request;
}

int Client::getResponseStatus() const {
    return _responseStatus;
}

void Client::setPort(int port) {
    _port = port;
}

void Client::setBuffer(const std::string& buffer) {
    _buffer += buffer;
    setRequest();
}

void Client::setRequest() {
    std::istringstream iss(_buffer);
	std::string line, header, body;
    int contentLength = 0;

	try {
        if (iss.str().find("\r\n") != std::string::npos \
                && _readStatus == READ_NOT_DONE) {
    		std::getline(iss, line);
	    	HttpRequest::parseRequestLine(_request, line);
            _readStatus = READ_LINE_DONE;
        }

		size_t pos = iss.str().find("\r\n\r\n");
        if (pos == std::string::npos &&\
            _readStatus == READ_LINE_DONE)
            return ;

		if (pos != std::string::npos &&\
             _readStatus == READ_LINE_DONE)
		{
			header = iss.str().substr(0, pos);
            HttpRequest::parseHeader(_request, header);
            if (_request._headers.find("Content-Length") != _request._headers.end())
                _request._contentLength = atoi(_request._headers["Content-Length"].c_str());
            else
                _request._contentLength = 0;
            if (_request._headers.find("Cookie") != _request._headers.end())
                HttpRequest::setCookie(_request);
            contentLength = _request._contentLength;
            _readStatus = READ_HEADER_DONE;
		}

		if ((_readStatus == READ_HEADER_DONE || \
                _readStatus == READ_BODY_DOING) && contentLength > 0)
        {
            body = iss.str().substr(pos + 4);
            contentLength -= body.length();
            if (contentLength > 0) {
                _readStatus = READ_BODY_DOING;
                return ;
            }
            else if (contentLength == 0)
                _readStatus = READ_DONE;
            else if (contentLength < 0)
                line.substr(0, line.length() + contentLength);
		}
        else if (_readStatus == READ_HEADER_DONE && \
                    _request._contentLength == 0) {
            _readStatus = READ_DONE;
        }
        _request._body += HttpRequest::parseBody(body);
		HttpRequest::isVaildRequest(_request);
        _responseStatus = 200;
	}
	catch (std::invalid_argument& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
		_readStatus = READ_ERROR;
        _responseStatus = 400;
	}
}

void Client::clearRequest()
{
    _request._method.clear();
    _request._uri.clear();
    _request._version.clear();
    _request._headers.clear();
    _request._cookie.clear();
    _request._body.clear();
    _request._contentLength = 0;
}

void Client::clearAll()
{
    clearRequest();
    _buffer.clear();
    _readStatus = READ_NOT_DONE;
    _responseStatus = 0;
}

void Client::printAllHeaders() const{
    const Headers& headers = _request._headers;
    for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}
