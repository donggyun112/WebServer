
#include "RequestHandle.hpp"

RequestHandle::RequestHandle(Port port) : _port(port) {
    clearAll();
	std::cout << "test2\n";
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

void RequestHandle::setBuffer(unsigned char *buffer, int length) {
    for (int i = 0; i < length; i++) {
        _buffer += buffer[i];
    }
	// std:
    setRequest();
}

bool mystrcmp(std::string str12, std::string str13) {
	char *str1 = (char *)str12.c_str();
	char *str2 = (char *)str13.c_str();
	while (*str1 || *str2) {
		if (*str1 != *str2) {
			std::cout << "str1: " << int(*str1) << std::endl;
			std::cout << "str2: " << int(*str2) << std::endl;
			std::cout << "Diff : " << *str1 - *str2 << std::endl;
			return 0;
		}
		str1++;
		str2++;
	}
	return 1;
}

void RequestHandle::setRequest() {
    std::string line, header, body;

	// if (_readStatus == READ_CHUNKED_BODY)
	// 	_readStatus = READ_BODY_DOING;

    if (_readStatus == READ_NOT_DONE && _request._method.empty()) {
        std::string::size_type pos = _buffer.find("\r\n");
        if (pos == std::string::npos) return;
		std::cout << "-------------------------1.5\n";
		// std::cout << "pos: " << pos << std::endl;
        line = _buffer.substr(0, pos);
		// std::cout << "line: " << line << std::endl;
        parseRequestLine(line);
		std::cout << "-------------------------1.6\n";
        _readStatus = READ_LINE_DONE;
		// _buffer.erase(0, pos + 2);
    }
std::cout << "-------------------------2\n";

    if (_readStatus == READ_LINE_DONE) {
        std::string::size_type headerEnd = _buffer.find("\r\n\r\n");
        if (headerEnd == std::string::npos) return;
        header = _buffer.substr(0, headerEnd);
		std::cout << "-------------------------2.5\n";
		// std::cout << _buffer << std::endl;
        parseHeader(header);
        _readStatus = READ_HEADER_DONE;
		// _buffer.erase(0, headerEnd + 4);
    }
std::cout << "-------------------------3\n";
// std::cout << "_readStatus: " << _readStatus << std::endl;

    if (_readStatus == READ_HEADER_DONE || _readStatus == READ_BODY_DOING || _readStatus == READ_CHUNKED_BODY) {

        std::string::size_type bodyStart = _buffer.find("\r\n\r\n");
        if (bodyStart == std::string::npos) return ;
        body = _buffer.substr(bodyStart + 4);

		// std::cout << "method: " << getMethod() << std::endl;
		// std::cout << "ch: " << getHeader("Transfer-Encoding") << std::endl;
		// std::cout << _buffer << std::endl;
		if (getHeader("Transfer-Encoding") == "chunked" || _readStatus == READ_CHUNKED_BODY){
			if (_readStatus == READ_BODY_DOING)
				std::cout << "chunked doing\n";
			// std::cout << "chunked\n";
            parseChunkedBody(body);
		}
        else {
			// std::cout << name << std::endl;
            parseRegularBody(body);
		}
    }
std::cout << "-------------------------4\n";

	if (_readStatus == READ_ERROR)
		throw BadRequest_400;
    // if (_readStatus == READ_DONE) {
        // if (_readStatus == READ_ERROR)
        // _responseStatus = 200;
    // }
}

void RequestHandle::parseRegularBody(std::string& body) {
	std::cout << "=parseRegularBody=\n";
    if (getMethod() == "GET" || (_request._contentLength == 0 && body.empty())) {
        _readStatus = READ_DONE;
        return;
    }
    if (body.size() == _request._contentLength) {
        _readStatus = READ_DONE;
        _request._body = body.substr(0, _request._contentLength);
		_buffer.clear();

    } else if (body.size() < _request._contentLength){
        _readStatus = READ_BODY_DOING;
		std::cout << "do do do " << std::endl;
        return ;
    } else if (body.size() > _request._contentLength) {
        // _readStatus = READ_ERROR;
		// std::cout << _buffer << std::endl;
		std::cout << "size over\n";
		std::cout << "body.size(): " << body.size() << std::endl;
		std::cout << "_request._contentLength: " << _request._contentLength << std::endl;
		throw PayloadTooLarge_413;
        // return ;
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
	_request._query.clear();
    _request._contentLength = 0;
}

void RequestHandle::clearAll()
{
	if (_readStatus == READ_CHUNKED_BODY)
		return ;
    clearRequest();
    _buffer.clear();
    _readStatus = READ_NOT_DONE;
    _isKeepAlive = true;
    _responseStatus = 0;
    _tempResult = "";
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
    std::string::size_type pos = uri.find("?");
    if (pos != std::string::npos) {
        _request._uri = uri.substr(0, pos);
        _request._query = uri.substr(pos + 1);
    }
    else
        _request._uri = uri;
}

void RequestHandle::parseRequestLine(const std::string& buf)
{
    std::istringstream iss(buf);
	std::string token, tmpUri;
	Config &Conf = Manager::config;

	iss >> token;
	if (iss.fail())
		throw MethodNotAllowed_405;
	_request._method = parseMethod(token);
	std::cout << "_readStatus: " << _readStatus << std::endl;
	std::cout << "method: " << _request._method << std::endl;
	// std::cout << "line: " << buf << std::endl;

	iss >> tmpUri;
	if (iss.fail()) {
		std::cout << "uri fail\n";
		std::cout << buf << std::endl;
		throw BadRequest_400;
	}
	parseUri(tmpUri);
	std::cout << "uri: " << _request._uri << std::endl;
	

	iss >> _request._version;
	if (iss.fail())
		throw HttpVersionNotSupported_505;
	std::cout << "version: " << _request._version << std::endl;
	_loc = Conf.getServerConfig(_port, this->getHost()).getLocation(_request._uri);	
	max_body_size = _loc.getMaxBodySize();
}

void RequestHandle::parseHeader(const std::string& buffer)
{
    std::istringstream iss(buffer);
    std::string line, key, value;
    std::string::size_type pos = 0;

	while (std::getline(iss, line))
	{
		pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			if (value.find("\r") != std::string::npos)
				value = value.substr(0, value.size() - 1);
			_request._headers[key] = value;
		}
	}
    if (_request._headers.find("Content-Length") != _request._headers.end()) {
        if (getMethod() == "GET") {
            _readStatus = READ_ERROR;
            return ;
        }
		// std::cout << "###################################################\n";
		// std::cout << "Content-Length: " << _request._headers["Content-Length"] << std::endl;
        _request._contentLength = atoi(_request._headers["Content-Length"].c_str());
		// std::cout << "Content-Length: " << _request._contentLength << std::endl;
		// std::cout << "max_body_size: " << max_body_size << std::endl;
		if (_request._contentLength > max_body_size)
			throw PayloadTooLarge_413;
    }
    else 
        _request._contentLength = 0;

    if (_request._headers.find("Cookie") != _request._headers.end())
        setCookie();
    _isKeepAlive = (_request._headers.find("Connection") == _request._headers.end() || _request._headers["Connection"] != "close");
    if (_request._contentLength == 0 && getHeader("Transfer-Encoding") != "chunked") {
        _readStatus = READ_DONE;
        validateRequest();
        _responseStatus = 200;
        return;
    }
}

void RequestHandle::parseChunkedBody(const std::string& body)
{
    std::istringstream iss(body);
    std::string chunk;
    size_t chunkLength;
    std::string res;

	if (body.find("0\r\n") == std::string::npos) {
		_readStatus = READ_CHUNKED_BODY;
		return;
	}
    while (std::getline(iss, chunk)) {
        if (!chunk.empty() && chunk.back() == '\r') {
            chunk.pop_back();
        }

        std::istringstream chunkStream(chunk);
        chunkStream >> std::hex >> chunkLength;

        if (!chunkStream) {
            _readStatus = READ_ERROR;
            return;
        }

        if (chunkLength == 0) {
            _readStatus = READ_DONE;
            _request._body = res;
            return;
        }

        std::string chunkData(chunkLength, '\0');
        if (chunkLength > chunkData.size()) {
            _readStatus = READ_CHUNKED_BODY;
            return ;
        }
        iss.read(&chunkData[0], chunkLength);
        res += chunkData;
        if (res.size() > max_body_size)
            throw PayloadTooLarge_413;
        iss.ignore(2);
    }

    if (!iss.eof()) {
        _readStatus = READ_CHUNKED_BODY;
        return;
    }
}

// void RequestHandle::parseChunkedBody(const std::string& body) v1
// {
//     std::cout << "cccccccccccccc\n";
//     std::istringstream iss(body);
//     std::string line, chunked;
//     size_t chunkLength;


//     while (std::getline(iss, line)) {
//         chunkLength = strtol(line.c_str(), NULL, 16);
//         if (chunkLength == 0) {
//             // 0\r\n 이후에는 \r\n이 한 번 더 나와야 합니다.
//             if (std::getline(iss, line) && line == "\r") {
//                 _request._body = chunked;
//                 _readStatus = READ_DONE;
//                 return;
//             } else {
//                 // 잘못된 형식의 chunked 데이터입니다.
//                 // 에러 처리 코드를 추가할 수 있습니다.
//                 throw BadRequest_400;
//             }
//         }
// 		std::cout << "max_body_size: " << max_body_size << std::endl;
//         if (chunkLength > max_body_size)
//             throw PayloadTooLarge_413;

//         // if (iss.eof()) {
// 		// 	std::cout << "sibal\n";
//         //     _readStatus = READ_CHUNKED_BODY;
//         //     return;
//         // }
// 		// std::cout << "test\n";
//         std::string chunk;
//         chunk.resize(chunkLength);
//         iss.read(&chunk[0], chunkLength);
//         chunked += chunk;
// 		if (chunkLength > chunked.size()) {
// 			_readStatus = READ_CHUNKED_BODY;
// 			return ;
// 		}
// 		_request._body += chunk;

//         // \r\n 읽기
//         std::getline(iss, line);
//         if (line != "\r") {
//             throw BadRequest_400;
//         }
//     }

//     // 0\r\n을 찾지 못한 경우, 아직 데이터가 완전히 도착하지 않았습니다.
//     _readStatus = READ_BODY_DOING;
// }


// void RequestHandle::parseChunkedBody(const std::string& body)
// {
// 	// std::cout << "cccccccccccccc\n";
//     std::istringstream iss(body);
//     std::string line;
//     long chunkLength;

//     // if (body.find("0\r\n") == std::string::npos) {
//     //     _readStatus = READ_BODY_DOING;
//     //     return;
//     // }

//     while (std::getline(iss, line)) {
//         chunkLength = strtol(line.c_str(), NULL, 16);
//         if (chunkLength == 0) {
//             _readStatus = READ_DONE;
//             return ;
//         }
//         getline(iss, line, '\r');
//         _request._body += line.substr(0, chunkLength);
//         getline(iss, line, '\n');
//     }
// }

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
    if (_readStatus == READ_ERROR)
        throw BadRequest_400;
}

void RequestHandle::printAllHeaders() const{
    const Headers& headers = _request._headers;
    for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

std::string RequestHandle::parsePart(const std::string& body, const std::string& boundary) {
    size_t start = body.find(boundary);
    if (start == std::string::npos) {
        return "";
    }
    start += boundary.length();
    size_t end = body.find(boundary, start);
    if (end == std::string::npos) {
        return "";
    }
    return body.substr(start, end - start);
}

std::string RequestHandle::parseFileContent(const std::string &body) {
    size_t start = body.find("\r\n\r\n");
    if (start == std::string::npos) {
        return "";
    }
    start += 4;
    return body.substr(start);
}

std::string RequestHandle::parseBodyHeader(const std::string& part) {
    size_t end = part.find("\r\n\r\n");
    if (end == std::string::npos) {
        return "";
    }
    return part.substr(0, end);
}

std::string RequestHandle::parseType(const std::string& body_header) {
    size_t start = body_header.find("Content-Type: ");
    if (start == std::string::npos) {
        return "";
    }
    start += 14;
    size_t end = body_header.find("\r\n", start);
    return body_header.substr(start, end - start);
}

std::string RequestHandle::parseFileName(const std::string& body_header) {
    size_t start = body_header.find("filename=\"");
    if (start != std::string::npos) {
        start += 10;
        size_t end = body_header.find("\"", start);
        return body_header.substr(start, end - start);
    }
    return "";
}

std::string RequestHandle::parseBoundary(const std::string& body_header) {
    size_t start = body_header.find("boundary=");
    if (start == std::string::npos) {
        return "";
    }
    start += 9;
    return body_header.substr(start);
}

std::string RequestHandle::parseContentType(std::string &body_header)
{
    size_t start = body_header.find("Content-Type: ");
    if (start == std::string::npos)
        return "";
    start += 14;
    size_t end = body_header.find(";", start);
    return body_header.substr(start, end - start);
}
