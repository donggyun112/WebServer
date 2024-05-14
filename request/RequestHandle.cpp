
#include "RequestHandle.hpp"

RequestHandle::RequestHandle(Port port) : _port(port) {
    clearAll();
}

RequestHandle::RequestHandle(const RequestHandle &Copy) : 
    _buffer(Copy.getBuffer()),
    _request(Copy.getRequest()),
    _port(Copy.getPort()),
    _readStatus(Copy._readStatus),
    _responseStatus(Copy._responseStatus) {}

RequestHandle::~RequestHandle() {}

std::string RequestHandle::getBuffer() const {
    return _buffer;
}

std::string RequestHandle::getMethod() const {
    return _request._method;
}

std::string RequestHandle::getUri() const {
    return _request._uri;
}

std::string RequestHandle::getVersion() const {
    return _request._version;
}

Port RequestHandle::getPort() const {
    return _port;
}

std::string RequestHandle::getHeader(const std::string& key) const {
    const Headers::const_iterator it = _request._headers.find(key);
    if (it == _request._headers.end())
        return "";
    return it->second;
}

std::string RequestHandle::getBody() const {
    return _request._body;
}

std::string RequestHandle::getCookie(const std::string& key) const {
    const Cookies::const_iterator it = _request._cookie.find(key);
    if (it == _request._cookie.end())
        return "";
    return it->second;
}

std::string RequestHandle::getHost() const {
	const Headers::const_iterator it = _request._headers.find("Host");
	if (it == _request._headers.end())
		return "";

	std::string host = it->second;
	size_t pos = host.find(":");
	if (pos != std::string::npos)
		host = host.substr(0, pos);
	return host;
}

const Request &RequestHandle::getRequest() const {
    return _request;
}

const std::string &RequestHandle::getQuery() const {
	return _request._query;
}

int RequestHandle::getResponseStatus() const {
    return _responseStatus;
}

void RequestHandle::setBuffer(const std::string& buffer) {
    _buffer += buffer;
    setRequest();
}

void RequestHandle::setRequest() {
    std::istringstream iss(_buffer);
	std::string line, header, body;

    if (iss.str().find("\r\n") == std::string::npos && _readStatus == READ_NOT_DONE)
        return ;
    
    if (iss.str().find("\r\n") != std::string::npos && _readStatus == READ_NOT_DONE) {
        std::getline(iss, line);
        HttpRequest::parseRequestLine(_request, line);
        _readStatus = READ_LINE_DONE;
    }
    size_t pos = iss.str().find("\r\n\r\n");
    if (pos == std::string::npos && _readStatus == READ_LINE_DONE)
        return ;
    if (pos != std::string::npos && _readStatus == READ_LINE_DONE)
    {
        header = iss.str().substr(0, pos);
        HttpRequest::parseHeader(_request, header);
        if (_request._headers.find("Content-Length") != _request._headers.end())
            _request._contentLength = atoi(_request._headers["Content-Length"].c_str());
        else 
            _request._contentLength = 0;
        if (_request._headers.find("Cookie") != _request._headers.end())
            HttpRequest::setCookie(_request);
        _readStatus = READ_HEADER_DONE;

    }
    if (getMethod() == "POST" && getHeader("Transfer-Encoding") == "chunked" && _readStatus == READ_HEADER_DONE) {
        body = iss.str().substr(pos + 4);
        if (body.find("0\r\n") == std::string::npos) {
            return ;
        }
        _readStatus = READ_DONE;
        _request._body = HttpRequest::setChunkedBody(body);
    }
    else if (_readStatus == READ_HEADER_DONE || _readStatus == READ_BODY_DOING) {
        if (_request._contentLength == 0) {
            _readStatus = READ_DONE;
            return ;
        }
        body = iss.str().substr(pos + 4);
        _request._currentLength = body.length();
        if (_request._currentLength < _request._contentLength) {
            _readStatus = READ_BODY_DOING;
            return ;
        } else if (_request._currentLength == _request._contentLength) {
            _readStatus = READ_DONE;
            _request._body = body;
        }
            else if (_request._currentLength > _request._contentLength)
            throw BadRequest_400;
    }
    HttpRequest::validateRequest(_request);
    _responseStatus = 200;
}

void RequestHandle::clearRequest()
{
    _request._method.clear();
    _request._uri.clear();
    _request._version.clear();
    _request._headers.clear();
    _request._cookie.clear();
    _request._body.clear();
	_request._query.clear();
    _request._contentLength = 0;
	_request._currentLength = 0;
}

void RequestHandle::clearAll()
{
    clearRequest();
    _buffer.clear();
    _readStatus = READ_NOT_DONE;
    _responseStatus = 0;

    //for test
    _tempResult = "";
}

void RequestHandle::printAllHeaders() const{
    const Headers& headers = _request._headers;
    for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}



