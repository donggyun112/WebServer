
#include "RequestHandle.hpp"

RequestHandle::RequestHandle(Port port) : _port(port) {
    clearAll();
}

RequestHandle::RequestHandle(const RequestHandle &Copy) : 
    _buffer(Copy.getBuffer()),
    _request(Copy.getRequest()),
    _port(Copy.getPort()),
    _readStatus(Copy._readStatus),
    _responseStatus(Copy._responseStatus),
    _isKeepAlive(Copy._isKeepAlive) {}

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

bool RequestHandle::getIsKeepAlive() const {
    return _isKeepAlive;
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

// void RequestHandle::setRequest() {
//     std::istringstream iss(_buffer);
// 	std::string line, header, body;

//     if (iss.str().find("\r\n") == std::string::npos && _readStatus == READ_NOT_DONE)
//         return ;
    
//     if (iss.str().find("\r\n") != std::string::npos && _readStatus == READ_NOT_DONE) {
//         std::getline(iss, line);
//         HttpRequest::parseRequestLine(_request, line);
//         _readStatus = READ_LINE_DONE;
//     }
//     size_t pos = iss.str().find("\r\n\r\n");
//     if (pos == std::string::npos && _readStatus == READ_LINE_DONE)
//         return ;
//     if (pos != std::string::npos && _readStatus == READ_LINE_DONE)
//     {
//         header = iss.str().substr(0, pos);
//         HttpRequest::parseHeader(_request, header);
//         if (_request._headers.find("Content-Length") != _request._headers.end())
//             _request._contentLength = atoi(_request._headers["Content-Length"].c_str());
//         else 
//             _request._contentLength = 0;
//         if (_request._headers.find("Cookie") != _request._headers.end())
//             HttpRequest::setCookie(_request);
//         if (_request._headers["Connection"] == "close")
//             _isKeepAlive = false;
//         _readStatus = READ_HEADER_DONE;

//     }
//     if (getMethod() == "POST" && getHeader("Transfer-Encoding") == "chunked" && _readStatus == READ_HEADER_DONE) {
//         body = iss.str().substr(pos + 4);
//         if (body.find("0\r\n") == std::string::npos) {
//             return ;
//         }
//         _readStatus = READ_DONE;
//         _request._body = HttpRequest::setChunkedBody(body);
//     }
//     else if (_readStatus == READ_HEADER_DONE || _readStatus == READ_BODY_DOING) {
//         if (_request._contentLength == 0) {
//             _readStatus = READ_DONE;
//             return ;
//         }
//         body = iss.str().substr(pos + 4);
//         _request._currentLength = body.length();
//         if (_request._currentLength < _request._contentLength) {
//             _readStatus = READ_BODY_DOING;
//             return ;
//         } else if (_request._currentLength == _request._contentLength) {
//             _readStatus = READ_DONE;
//             _request._body = body;
//         }
//             else if (_request._currentLength > _request._contentLength)
//             throw BadRequest_400;
//     }
//     HttpRequest::validateRequest(_request);
//     _responseStatus = 200;
// }

void RequestHandle::setRequest()
{
    std::string buffer(_buffer);
    std::string::size_type pos = buffer.find("\r\n");
    if (pos == std::string::npos && _readStatus == READ_NOT_DONE)
        return ;
    std::string line, header, body;
    line = buffer.substr(0, pos);
    parseRequestLine(line);
    _readStatus = READ_LINE_DONE;
    pos += 2;

    // pos = buffer.find("\r\n\r\n");
    std::string::size_type headerEnd = buffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos && _readStatus == READ_LINE_DONE)
        return ;

    header = buffer.substr(pos, headerEnd - pos);
    // std::cout << "----------------\n" << header << "\n----------------" << std::endl;
    parseHeader(header);

    _readStatus = READ_HEADER_DONE;

    // if () 조건 전체적으로 추가해야함 여러번 호출하면 옳게 파싱하지 못함. 
    body = buffer.substr(pos + 4);
    if (getMethod() == "POST" && getHeader("Transfer-Encoding") == "chunked")
        parseChunkedBody(body);
    else
        ParseRegularBody(body);
    validateRequest();
    _responseStatus = 200;
}

std::string RequestHandle::parseMethod(const std::string& methodStr)
{
	if (methodStr == "GET")
		return "GET";
	else if (methodStr == "POST")
		return "POST";
	else if (methodStr == "DELETE")
		return "DELETE";
	else
		return "OTHER";
}

void RequestHandle::parseUri(const std::string& uri)
{
	std::string path;
    size_t pos = uri.find('?');

	if (pos != std::string::npos) {
        path = uri.substr(0, uri.find('?'));
        _request._query = uri.substr(uri.find('?') + 1);
	}
	else 
		_request._uri = uri;
}

void RequestHandle::parseRequestLine(const std::string& buf)
{
    std::istringstream iss(buf);
	std::string token, tmpUri;

	iss >> token;
	if (iss.fail())
		throw MethodNotAllowed_405;
	_request._method = parseMethod(token);

	iss >> tmpUri;
	if (iss.fail())
		throw BadRequest_400;
	parseUri(tmpUri);

	iss >> _request._version;
	if (iss.fail())
		throw HttpVersionNotSupported_505;
}

/*

void HttpRequest::parseHeader(Request &req, const std::string& header)
{
	std::istringstream iss(header);
	std::string line;

	while (std::getline(iss, line))
	{
		std::string::size_type pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			req._headers[key] = value;
		}
	}
}
*/

void RequestHandle::parseHeader(const std::string& buffer)
{
    std::istringstream iss(buffer);
    std::string line, key, value;
    // std::string::size_type pos = 0;

	while (std::getline(iss, line))
	{
		std::string::size_type pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			_request._headers[key] = value;
		}
	}
    if (_request._headers.find("Content-Length") != _request._headers.end())
        _request._contentLength = atoi(_request._headers["Content-Length"].c_str());
    else 
        _request._contentLength = 0;
    if (_request._headers.find("Cookie") != _request._headers.end())
        HttpRequest::setCookie(_request);
    _isKeepAlive = (_request._headers.find("Connection") == _request._headers.end() || _request._headers["Connection"] != "close");
}

void RequestHandle::ParseRegularBody(const std::string& body)
{
    if (_request._contentLength == 0) {
        _readStatus = READ_DONE;
        return ;
    }

    size_t length = body.length();
    if (length < _request._contentLength) {
        _readStatus = READ_BODY_DOING;
        return ;
    }
    if (length > _request._contentLength)
        throw BadRequest_400;
    _request._body = body.substr(0, _request._contentLength);
    _readStatus = READ_DONE;
}

void RequestHandle::parseChunkedBody(const std::string& body)
{
    std::istringstream iss(body);
    std::string chunk;
    size_t chunkLength;

    while (std::getline(iss, chunk)) {
        if (chunk == "0") {
            _readStatus = READ_DONE;
            return ;
        }
        std::istringstream chunk_iss(chunk);
        chunk_iss >> std::hex >> chunkLength;
        if (!chunk_iss) {
            _readStatus = READ_ERROR;
            return ;
        }
        getline(iss, chunk, '\r');
        _request._body += chunk.substr(0, chunkLength);
        getline(iss, chunk, '\n');
    }
}

void RequestHandle::setCookie()
{
	std::istringstream iss(_request._headers["Cookie"]);
	std::string line;

	while (std::getline(iss, line, ';')) {
		std::string::size_type pos = line.find("=");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			_request._cookie[key] = value;
		}
	}
}

void RequestHandle::validateRequest()
{
	if (_request._method == "OTHER")
		throw MethodNotAllowed_405;
	if (_request._uri.empty())
		throw BadRequest_400;
	if (_request._version != "HTTP/1.1")
		throw HttpVersionNotSupported_505;
	if (_request._headers.find("Host") == _request._headers.end())
		throw BadRequest_400;
}

//----------------------------
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
}

void RequestHandle::clearAll()
{
    clearRequest();
    _buffer.clear();
    _readStatus = READ_NOT_DONE;
    _isKeepAlive = true;
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