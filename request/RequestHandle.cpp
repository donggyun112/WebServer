
#include "RequestHandle.hpp"

std::map<int, std::string> Error::errors;

enum Method {
    GET = 1,
    POST,
    PUT,
    DELETE,
};

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

void RequestHandle::setRequest() {
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

void RequestHandle::clearRequest()
{
    _request._method.clear();
    _request._uri.clear();
    _request._version.clear();
    _request._headers.clear();
    _request._cookie.clear();
    _request._body.clear();
    _request._contentLength = 0;
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

bool RequestHandle::Utils::isDirectory(const std::string &path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {

        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool RequestHandle::Utils::isValidPath(const std::string &path) {
    // 경로가 비어있는 경우
    if (path.empty()) {
        return false;
    }
    // 경로가 너무 긴 경우
    if (path.length() > PATH_MAX) {
        return false;
    }
    // 경로에 불법적인 문자가 포함된 경우
    if (path.find_first_of("\0\\") != std::string::npos) {
        return false;
    }
    // 경로가 상대경로인 경우
    if (path[0] != '/') {
        return false;
    }
    return true;
}

std::string RequestHandle::Utils::normalizePath(const std::string &path) {
    std::string normalizedPath = path;
    
    // 연속된 슬래시를 하나의 슬래시로 치환
    std::string::size_type pos = 0;
    while ((pos = normalizedPath.find("//", pos)) != std::string::npos) {
        normalizedPath.erase(pos, 1);
    }
    
    // 경로의 마지막 문자가 '/'인 경우 제거
    if (!normalizedPath.empty() && normalizedPath[normalizedPath.length() - 1] == '/') {
        normalizedPath.erase(normalizedPath.length() - 1);
    }
    
    // 상대경로 요소 제거
    while ((pos = normalizedPath.find("/../")) != std::string::npos) {
        if (pos == 0) {
            return "";
        }
        std::string::size_type prevPos = normalizedPath.rfind('/', pos - 1);
        if (prevPos == std::string::npos) {
            return "";
        }
        normalizedPath.erase(prevPos, pos - prevPos + 3);
    }
    
    // 현재 디렉토리 요소 제거
    while ((pos = normalizedPath.find("/./")) != std::string::npos) {
        normalizedPath.erase(pos, 2);
    }
    
    return normalizedPath;
}

int RequestHandle::Utils::getMethodNumber(const std::string &method) {
	if (method == "GET")
		return 1;
	else if (method == "POST")
		return 2;
	else if (method == "PUT")
		return 3;
	else if (method == "DELETE")
		return 4;
	return 0;
}

std::string RequestHandle::Utils::getCurTime() {
	struct timeval tv;
	struct tm *tm;
	char buf[64];

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm);
	return std::string(buf);
}

bool RequestHandle::Utils::isExist(const std::string& path)
{
	std::string full_path = path;
	std::ifstream ifs(full_path.c_str());
	
	return ifs.is_open();
}

std::string RequestHandle::Utils::getContentType(const std::string &extension) {
    if (extension == "html")
        return "text/html; charset=utf-8";
    else if (extension == "css")
        return "text/css";
    // 다른 확장자에 대한 Content-Type 매핑 추가
    else if (extension == "png")
		return "image/png";
	else if (extension == "jpg")
		return "image/jpeg";
	else if (extension == "jpeg")
		return "image/jpeg";
	else if (extension == "gif")
		return "image/gif";
	else if (extension == "bmp")
		return "image/bmp";
	else if (extension == "ico")
		return "image/x-icon";
	else if (extension == "svg")
		return "image/svg+xml";
	else if (extension == "js")
		return "application/javascript";
	else if (extension == "json")
		return "application/json";
	else if (extension == "pdf")
		return "application/pdf";
	else if (extension == "zip")
		return "application/zip";
	else
        return "application/octet-stream";
}

std::string RequestHandle::Utils::nomralizeUrl(const std::string &HTTP_uri) {
	std::string normalizedUrl = HTTP_uri;
	
	// //제거
	std::string::size_type pos = 0;
	while ((pos = normalizedUrl.find("//", pos)) != std::string::npos) {
		normalizedUrl.erase(pos, 1);
	}
	return normalizedUrl;	
}

bool    RequestHandle::Utils::isExtention(std::string httpPath) { 
    if (httpPath.find_last_of('.') == std::string::npos)
        return false;
    return true;
}

Response RequestHandle::createErrorResponse(int status, const std::string errorMessage) {
	Response response;
	response.setStatusCode(status);
	response.setHeader("Content-Type", "text/html; charset=utf-8");
	response.setHeader("Date", Utils::getCurTime());
	std::string errorBody = "<html><body><h1>" + web::toString(status) + " " + Error::errors[status] + "</h1><p>" + errorMessage + "</p></body></html>";
	response.setBody(errorBody);
	response.setHeader("Content-Length", web::toString(errorBody.length()));
	response.setHeader("Connection", "close");
	return response;

}

std::string RequestHandle::Utils::getFilePath(const std::string &serverRoot, const std::string &httpUri, const LocationConfig &loc) {
    std::string filePath;
    std::string alias = loc.getAlias();

    if (!alias.empty() && httpUri.find(alias) == 0) {
        filePath = alias + httpUri.substr(alias.length());
    }
    else if (isExtention(httpUri) == true) {
        filePath = serverRoot + loc.getRoot() + httpUri;
    } else if (isDirectory(serverRoot + httpUri) == true) {

        filePath = serverRoot + httpUri;
    } else {
        filePath = serverRoot + loc.getRoot() + httpUri.substr(loc.getRoot().length(), httpUri.length() - loc.getRoot().length());
    }

    return filePath;
}


Response RequestHandle::handleRedirect(const LocationConfig &location) {
    Response response;
    std::string returnCode = location.getReturnCode();
    std::string returnUrl = location.getReturnUrl();

    if (!returnCode.empty() && !returnUrl.empty()) {
        int statusCode = std::stoi(returnCode);
        response.setStatusCode(statusCode);
        response.setHeader("Location", returnUrl);
        response.setHeader("Connection", "close");
		response.set_redirect(returnUrl, statusCode);
		return response;
    }
	std::cout << "Redirected to: " << returnUrl << std::endl;
	response.setStatusCode(OK_200);
	std::cout << response.getStatusCode() << std::endl;
    return response;
}

std::string RequestHandle::Utils::getFileExtension(const std::string &filePath) {
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filePath.substr(dotPos + 1);
    }
    return "";
}

std::streamsize RequestHandle::Utils::getFileSize(std::ifstream &file) {
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    return fileSize;
}

std::string RequestHandle::Utils::readFileContent(std::ifstream &file, std::streamsize fileSize) {
    std::string content(fileSize, '\0');
    file.read(&content[0], fileSize);
    return content;
}

Response RequestHandle::handleGetRequest(const Config &Conf) {
    Response response;
    // URL 정규화
    std::string HTTP_uri = Utils::nomralizeUrl(_request._uri);
	const std::string Server_root = Utils::normalizePath(Conf[this->getPort()].getPath());


    if (Server_root.empty()) {
		return createErrorResponse(InternalServerError_500, "Server configuration error: root directory not set.");
    }

    // Location 블록 설정 확인
    LocationConfig loc;
    try {
        loc = Conf[_port].getLocation(HTTP_uri);
    } catch (const std::exception &e) {
        loc = Conf[_port].getLocation("/");
		if (Utils::isMethodPossible(GET, loc) == false) {
        	return handleMethodNotAllowed();
    	}
    }

    std::string filePath = Utils::getFilePath(Server_root, HTTP_uri, loc);

    // 경로 유효성 검사
    if (!Utils::isValidPath(filePath)) {
		return createErrorResponse(BadRequest_400, "Invalid request path.");
    }

    
	Response redirectResponse = handleRedirect(loc);
	if (redirectResponse.getStatusCode() != OK_200) {
		return redirectResponse;
	}

    // 인덱스 파일 설정
    std::string index = loc.getIndex();
    if (Utils::isDirectory(filePath) && !index.empty()) {
        filePath += "/" + index;
    }

    // 파일 확장자 추출
    std::string extension = Utils::getFileExtension(filePath);
    // 파일 읽기
    std::ifstream file(filePath.c_str(), std::ios::binary);

    if (file.is_open() && file.good()) {
        // 파일 크기 확인
        std::streamsize fileSize = Utils::getFileSize(file);

        // 파일 크기 제한 설정
        const std::streamsize maxFileSize = 10 * 1024 * 1024;
        if (fileSize > maxFileSize) {
			return createErrorResponse(PayloadTooLarge_413, "The requested file is too large.");
        }
        // 파일 내용 읽기
        std::string body = Utils::readFileContent(file, fileSize);
        file.close();

        response.setStatusCode(OK_200);
        response.setHeader("Date", Utils::getCurTime());
        response.setHeader("Content-Type", Utils::getContentType(extension));
        response.setBody(body);
        response.setHeader("Content-Length", web::toString(body.length()));
        response.setHeader("Connection", "keep-alive");

    } else {
        if (Utils::isDirectory(filePath)) {
			if (loc.getAutoindex() == true) {
            	handleAutoIndex(response, filePath);
			} else {
				return createErrorResponse(Forbidden_403, "Directory listing not allowed.");
			}
        } else {
			if (loc.getAutoindex() == false) {
				return createErrorResponse(NotFound_404, "The requested file was not found.");
			} else {
				handleAutoIndex(response, filePath.substr(0, filePath.find_last_of('/')));
			}
		}
    }
    response.setHeader("Server", "42Webserv");
    return response;
}

Response RequestHandle::handleMethodNotAllowed() {
    Response response;
    response.setStatusCode(MethodNotAllowed_405);
    response.setHeader("Content-Type", "text/html");
    response.setHeader("Date", Utils::getCurTime());

    std::string errorBody = "<html><body><h1>405 Method Not Allowed</h1><p>The requested method is not allowed.</p></body></html>";
    response.setBody(errorBody);
    response.setHeader("Content-Length", web::toString(errorBody.length()));
    response.setHeader("Connection", "close");

    return response;
}

bool    RequestHandle::Utils::isMethodPossible(int method, const LocationConfig &Loc) {
    for (size_t i = 0; i < Loc.getAllowMethods().size(); ++i) {
        if (method == Utils::getMethodNumber(Loc.getAllowedMethod(i)))
            return true;
    }
    return false;
}

Response RequestHandle::sendResponse(const Config &Conf) {


    int method = Utils::getMethodNumber(_request._method);
    switch (method) {
        case GET:
            return handleGetRequest(Conf);
        case POST:
            // return handlePostRequest(Conf);
        case DELETE:
            // return handleDeleteRequest(Conf);
        default:
            return handleMethodNotAllowed();
    }
}

std::string RequestHandle::execute(const Config &Conf) {
	Response response = sendResponse(Conf);
	std::string responseStr = response.get_responses();
	_tempResult = responseStr;
	return responseStr;
}


std::string FormatTime(time_t time)
{
	char buffer[80];
	struct tm *timeinfo = localtime(&time);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
	return std::string(buffer);
}

std::string FormatSize(double size)
{
	const char *sizes[] = { "B", "KB", "MB", "GB", "TB" };
	int i = 0;
	while (size > 1024)
	{
		size /= 1024;
		i++;
	}

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) <<size << sizes[i];
	return oss.str();
}

void RequestHandle::handleAutoIndex(Response &response, const std::string &servRoot)
{
    std::string dirPath = servRoot;

    struct stat fileStat;
    std::stringstream body;
    body << "<html>\n<head>\n<title> AutoIndex </title>\n</head>\n<body>\n";
    body << "<h1>Index of / </h1>\n";
	body << "<hr> <pre>\n<table>\n<tr><th></th><th></th><th></th></tr>\n";

    DIR *dir = opendir(dirPath.c_str());
	if (dir == NULL) {
        response.setStatusCode(NotFound_404);
        response.setHeader("Content-Type", "text/html; charset=utf-8");
        response.setHeader("Date", Utils::getCurTime());
        std::string errorBody = "<html><body><h1>404 Not Found</h1><p>Directory listing not allowed.</p></body></html>";
        response.setBody(errorBody);
        response.setHeader("Content-Length", web::toString(errorBody.length()));
        response.setHeader("Connection", "close");
	}
    if (dir)
    {
        std::vector<std::string> fileList;
        struct dirent *ent;
        size_t maxFileNameLength = 0;
        while ((ent = readdir(dir)) != NULL) {
            fileList.push_back(ent->d_name);
            maxFileNameLength = std::max(maxFileNameLength, strlen(ent->d_name));
        }
        closedir(dir);

        std::sort(fileList.begin(), fileList.end());

        int count = fileList.size();
        for (int i = 0; i < count; i++)
        {
            std::string fileName = fileList[i];
            if (fileName == ".")
                continue;
            std::string filePath = dirPath + "/" + fileName;
            if (stat(filePath.c_str(), &fileStat) == -1) {
                _responseStatus = 503;
                return ;
            }
            if (stat(filePath.c_str(), &fileStat) == 0)
            {
                body << "<tr>" << "<td>";
                if (S_ISDIR(fileStat.st_mode))
                    body << "<a href=\"" << fileName << "/\">" << std::left << fileName + "/" << "</a>";
                else
                    body << std::setw(maxFileNameLength + 1) << std::left << fileName;
                body << "</td><td>\t\t" << FormatTime(fileStat.st_mtime) << "</td>";
                double fileSize = static_cast<double>(fileStat.st_size);
                body << "<td>\t\t" << FormatSize(fileSize) << "</td>" << "</tr>\n";
            }
            else {
                response.setStatusCode(InternalServerError_500);
                response.setHeader("Content-Type", "text/html; charset=utf-8");
                response.setHeader("Date", Utils::getCurTime());
                std::string errorBody = "<html><body><h1>500 Internal Server Error</h1><p>Directory listing not allowed.</p></body></html>";
                response.setBody(errorBody);
                response.setHeader("Content-Length", web::toString(errorBody.length())); // C++11 버전입니다.
                response.setHeader("Connection", "close");
                return ;
            }
        }
		body << " </table> </pre>\n<hr>\n</body>\n</html>\n";
        response.setStatusCode(OK_200);
        response.setHeader("Date", Utils::getCurTime());
        response.setHeader("Content-Type", "text/html");
        response.setBody(body.str());
        response.setHeader("Content-Length", web::toString(body.str().length())); // C++11 버전입니다.
        response.setHeader("Connection", "close");
    }
}