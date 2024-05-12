
#include "RequestHandle.hpp"

std::map<int, std::string> Error::errors;



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
	// return it->second;
}

const Request &RequestHandle::getRequest() const {
    return _request;
}

int RequestHandle::getResponseStatus() const {
    return _responseStatus;
}

void RequestHandle::setBuffer(const std::string& buffer) {
    _buffer += buffer;
    setRequest();
}

void printAllEnv() {
	std::cout << "REQUEST_METHOD: " << getenv("REQUEST_METHOD") << std::endl;
	std::cout << "REQUEST_URI: " << getenv("REQUEST_URI") << std::endl;
	std::cout << "QUERY_STRING: " << getenv("QUERY_STRING") << std::endl;
	std::cout << "SCRIPT_NAME: " << getenv("SCRIPT_NAME") << std::endl;
	std::cout << "PATH_INFO: " << getenv("PATH_INFO") << std::endl;
	std::cout << "SERVER_NAME: " << getenv("SERVER_NAME") << std::endl;
	std::cout << "SERVER_PORT: " << getenv("SERVER_PORT") << std::endl;
	std::cout << "HTTP_HOST: " << getenv("HTTP_HOST") << std::endl;
	std::cout << "HTTP_USER_AGENT: " << getenv("HTTP_USER_AGENT") << std::endl;
	std::cout << "HTTP_ACCEPT: " << getenv("HTTP_ACCEPT") << std::endl;
	std::cout << "HTTP_ACCEPT_LANGUAGE: " << getenv("HTTP_ACCEPT_LANGUAGE") << std::endl;
	std::cout << "HTTP_ACCEPT_ENCODING: " << getenv("HTTP_ACCEPT_ENCODING") << std::endl;
	std::cout << "HTTP_ACCEPT_CHARSET: " << getenv("HTTP_ACCEPT_CHARSET") << std::endl;
	std::cout << "HTTP_KEEP_ALIVE: " << getenv("HTTP_KEEP_ALIVE") << std::endl;
	std::cout << "HTTP_CONNECTION: " << getenv("HTTP_CONNECTION") << std::endl;
	std::cout << "HTTP_REFERER: " << getenv("HTTP_REFERER") << std::endl;
	std::cout << "HTTP_COOKIE: " << getenv("HTTP_COOKIE") << std::endl;
	std::cout << "HTTP_CONTENT_TYPE: " << getenv("HTTP_CONTENT_TYPE") << std::endl;
	std::cout << "HTTP_CONTENT_LENGTH: " << getenv("HTTP_CONTENT_LENGTH") << std::endl;

}

void RequestHandle::setRequest() {
    std::istringstream iss(_buffer);
	std::string line, header, body;
	std::cout << "Status: " << _readStatus << std::endl;
	std::cout << "what ???\n";
	try {
        if (iss.str().find("\r\n") != std::string::npos \
                && _readStatus == READ_NOT_DONE) {
    		std::getline(iss, line);
			std::cout << "what ???\n";
	    	HttpRequest::parseRequestLine(_request, line);
            _readStatus = READ_LINE_DONE;
        }

		size_t pos = iss.str().find("\r\n\r\n");
		std::cout << "what ???\n";
        if (pos == std::string::npos &&\
            _readStatus == READ_LINE_DONE)	
            return ;
		std::cout << "what ???3\n";
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
            _readStatus = READ_HEADER_DONE;
			std::cout << "3\n";
		}
		std::cout << "what ???2\n";
		std::cout << "Content-Length: " << _request._contentLength << std::endl;
		if (_readStatus == READ_HEADER_DONE || _readStatus == READ_BODY_DOING ) // 수정해야됨
        {
			std::cout << "4\n";
            body = iss.str().substr(pos + 4);
			std::cout << "Body: " << body << std::endl;
            _request._currentLength += body.length();
            if (_request._currentLength < _request._contentLength ) {
                _readStatus = READ_BODY_DOING;
				std::cout << "Not yet done | \ncontent-Length : " << _request._contentLength << "current-Length : " << _request._contentLength << std::endl;
                return ;
            }
            else if (_request._currentLength == _request._contentLength)
                _readStatus = READ_DONE;
            else if (_request._currentLength > _request._contentLength)
                _responseStatus = 404;
		}
        else if (_request._contentLength == _request._currentLength) {
            _readStatus = READ_DONE;
			std::cout << "what ???1\n";
        }
        _request._body += HttpRequest::parseBody(body);
		HttpRequest::isVaildRequest(_request);
        _responseStatus = 200;
	}
	catch (std::invalid_argument& e) {
		std::cout << "6\n";
        std::cerr << "Exception caught: " << e.what() << std::endl;
		_readStatus = READ_ERROR;
        _responseStatus = 400;
	}
	std::cout << "7th + readStatus = " << _readStatus << std::endl;;
	if (_readStatus == READ_DONE) {
		setEnv();
		printAllEnv();
	}
}

void RequestHandle::clearRequest()
{
    _request._method.clear();
    _request._uri.clear();
    _request._version.clear();
    _request._headers.clear();
    _request._cookie.clear();
    _request._body.clear();
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



void	RequestHandle::setEnv() {
	std::string host = getHost();
	std::string uri = getUri();
	std::string path = uri;
	std::string query = "";
	std::string scriptName = "";
	std::string queryString = "";
	std::string requestUri = uri;
	std::string requestMethod = getMethod();
	std::string serverName = host;
	std::string serverPort = web::toString(getPort());
	std::string body = getBody();

	if (uri.find('?') != std::string::npos) {
		query = uri.substr(uri.find('?') + 1);
		path = uri.substr(0, uri.find('?'));
	}
	if (path.find('/') != std::string::npos) {
		scriptName = path.substr(0, path.find_last_of('/'));
		path = path.substr(path.find_last_of('/'));
	}
	if (query.find('&') != std::string::npos) {
		queryString = query.substr(query.find('&') + 1);
		query = query.substr(0, query.find('&'));
	}
	setenv("REQUEST_METHOD", requestMethod.c_str(), 1);
	setenv("REQUEST_URI", requestUri.c_str(), 1);
	setenv("QUERY_STRING", query.c_str(), 1);
	setenv("SCRIPT_NAME", scriptName.c_str(), 1);
	setenv("PATH_INFO", path.c_str(), 1);
	setenv("QUERY_STRING", query.c_str(), 1);
	setenv("SERVER_NAME", serverName.c_str(), 1);
	setenv("SERVER_PORT", serverPort.c_str(), 1);
	setenv("HTTP_HOST", host.c_str(), 1);
	setenv("HTTP_USER_AGENT", getHeader("User-Agent").c_str(), 1);
	setenv("HTTP_ACCEPT", getHeader("Accept").c_str(), 1);
	setenv("HTTP_ACCEPT_LANGUAGE", getHeader("Accept-Language").c_str(), 1);
	setenv("HTTP_ACCEPT_ENCODING", getHeader("Accept-Encoding").c_str(), 1);
	setenv("HTTP_ACCEPT_CHARSET", getHeader("Accept-Charset").c_str(), 1);
	setenv("HTTP_KEEP_ALIVE", getHeader("Keep-Alive").c_str(), 1);
	setenv("HTTP_CONNECTION", getHeader("Connection").c_str(), 1);
	setenv("HTTP_REFERER", getHeader("Referer").c_str(), 1);
	setenv("HTTP_COOKIE", getHeader("Cookie").c_str(), 1);
	setenv("HTTP_CONTENT_TYPE", getHeader("Content-Type").c_str(), 1);
	setenv("HTTP_CONTENT_LENGTH", getHeader("Content-Length").c_str(), 1);
	setenv("HTTP_BODY", body.c_str(), 1);

}

void RequestHandle::printAllHeaders() const{
    const Headers& headers = _request._headers;
    for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}



