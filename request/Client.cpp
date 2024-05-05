#include "Client.hpp"
#include "../tmp/test.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

Client::Client(Port port) : _port(port) {
    clearAll();
}

Client::Client(const Client &Copy) : 
    _buffer(Copy.getBuffer()),
    _request(Copy.getRequest()),
    _port(Copy.getPort()),
    _readStatus(Copy._readStatus),
    _responseStatus(Copy._responseStatus) {}

Client::~Client() {}

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

Port Client::getPort() const {
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

    //for test
    _tempResult = "";
}

void Client::printAllHeaders() const{
    const Headers& headers = _request._headers;
    for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}



#include <sys/stat.h>
#include <unistd.h>
#include <climits>
#include <cstring>

bool isDirectory(const std::string &path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool isValidPath(const std::string &path) {
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

std::string normalizePath(const std::string &path) {
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

int getMethodNumber(const std::string &method) {
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

#include <sys/time.h>

std::string getCurTime() {
	struct timeval tv;
	struct tm *tm;
	char buf[64];

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm);
	return std::string(buf);
}

bool isExist(const std::string& path)
{
	std::string full_path = path;
	std::ifstream ifs(full_path.c_str());
	
	return ifs.is_open();
}

std::string getContentType(const std::string &extension) {
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

std::string nomralizeUrl(const std::string &HTTP_uri) {
	std::string normalizedUrl = HTTP_uri;
	
	// //제거
	std::string::size_type pos = 0;
	while ((pos = normalizedUrl.find("//", pos)) != std::string::npos) {
		normalizedUrl.erase(pos, 1);
	}
	return normalizedUrl;	
}
Response handleMethodNotAllowed();

bool    isExtention(std::string httpPath) { 
    if (httpPath.find_last_of('.') == std::string::npos)
        return false;
    return true;
}



Response Client::handleGetRequest(const Config &Conf) {
    Response response;
    std::string HTTP_uri = this->getUri();
    std::string filePath;

    // URL 정규화
    HTTP_uri = nomralizeUrl(HTTP_uri);

    // 가상 서버 설정에 따른 루트 디렉토리 설정
    // std::string Server_root = Conf[_port].getRoot();
	std::string Server_root = Conf[this->getPort()].getPath();
    if (Server_root.empty()) {
        response.setStatusCode(InternalServerError_500);
        response.setHeader("Content-Type", "text/html; charset=utf-8");
        response.setHeader("Date", getCurTime());
        std::string errorBody = "<html><body><h1>500 Internal Server Error</h1><p>Server configuration error: root directory not set.</p></body></html>";
        response.setBody(errorBody);
        response.setHeader("Content-Length", std::to_string(errorBody.length()));
        response.setHeader("Connection", "close");
        return response;
    }



    // Location 블록 설정 확인
    LocationConfig loc;
    try {
        loc = Conf[_port].getLocation(HTTP_uri);
    } catch (const std::exception &e) {
        loc = Conf[_port].getLocation("/");
    }


    //파일인지, 디렉토리인지, 특수파일인지 확인?
    if (isExtention(HTTP_uri) == true) {
        filePath = Server_root + loc.getRoot() + HTTP_uri;
    } else if (isDirectory(HTTP_uri) == true) {
        filePath = Server_root + HTTP_uri;
    } else
        filePath = Server_root + loc.getRoot() + HTTP_uri.substr(loc.getRoot().length(), HTTP_uri.length());

    std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    filePath = normalizePath(filePath);

    // 경로 유효성 검사
    if (!isValidPath(filePath)) {
        response.setStatusCode(BadRequest_400);
        response.setHeader("Content-Type", "text/html; charset=utf-8");
        response.setHeader("Date", getCurTime());
        std::string errorBody = "<html><body><h1>400 Bad Request</h1><p>Invalid request path.</p></body></html>";
        response.setBody(errorBody);
        response.setHeader("Content-Length", std::to_string(errorBody.length()));
        response.setHeader("Connection", "close");
        return response;
    }
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    // HTTP 메서드 허용 검사
    std::vector<std::string> allowMethods = loc.getAllowMethods();
    if (!allowMethods.empty() && std::find(allowMethods.begin(), allowMethods.end(), "GET") == allowMethods.end()) {
        return handleMethodNotAllowed();
    }
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    // 리다이렉션 처리
    std::string returnCode = loc.getReturnCode();
    std::string returnUrl = loc.getReturnUrl();
    if (!returnCode.empty() && !returnUrl.empty()) {
        int statusCode = std::stoi(returnCode);
        response.setStatusCode(statusCode);
        response.setHeader("Location", returnUrl);
        response.setHeader("Connection", "close");
        return response;
    }
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    // 루트 디렉토리 설정
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    // 별칭 설정
    std::string alias = loc.getAlias();
    if (!alias.empty()) {
        if (HTTP_uri.find(alias) == 0) {
            filePath = alias + HTTP_uri.substr(alias.length());
        }
    }
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    // 인덱스 파일 설정
    std::string index = loc.getIndex();

    if (isDirectory(filePath)) {
        if (!index.empty()) {
            filePath += "/" + index;
        } else {
            bool autoindex = loc.getAutoindex();
            if (autoindex) {
                // 디렉토리 리스팅 구현
                // std::string autoIndexBody = handleAutoIndex(filePath);
                // response.setStatusCode(OK_200);
                // response.setHeader("Date", getCurTime());
                // response.setHeader("Content-Type", "text/html");
                // response.setBody(autoIndexBody);
                // response.setHeader("Content-Length", std::to_string(autoIndexBody.length()));
                // response.setHeader("Connection", "close");
                // return response;
                std::cout << "filePath: " << __LINE__ << filePath << std::endl;
            } else {
                response.setStatusCode(Forbidden_403);
                response.setHeader("Content-Type", "text/html; charset=utf-8");
                response.setHeader("Date", getCurTime());
                std::string errorBody = "<html><body><h1>403 Forbidden</h1><p>Directory listing not allowed.</p></body></html>";
                response.setBody(errorBody);
                response.setHeader("Content-Length", std::to_string(errorBody.length()));
                response.setHeader("Connection", "close");
                std::cout << "filePath: " << __LINE__ << filePath << std::endl;
                return response;
            }
        }
    }
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    // try_files 설정
    std::vector<std::string> tryFiles = loc.getTryFiles();
    if (!tryFiles.empty()) {
        for (std::vector<std::string>::const_iterator it = tryFiles.begin(); it != tryFiles.end(); ++it) {
            std::string tryFile = *it;
            if (tryFile == "$uri") {
                tryFile = filePath;

            } else if (tryFile == "$uri/") {
                tryFile = filePath + "/";

            }

            if (isExist(tryFile)) {
                filePath = tryFile;

                break;
            }
        }
    }
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    // 파일 확장자 추출
    std::string extension = filePath.substr(filePath.find_last_of(".") + 1);

    // 파일 읽기
    std::ifstream file(filePath.c_str(), std::ios::binary);
    std::cout << "Ssssssssssssssasdasadasfasfafafaf" << filePath << std::endl;
    if (file.is_open()) {
        // 파일 크기 확인
        file.seekg(0, std::ios::end);
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // 파일 크기 제한 설정
        const std::streamsize maxFileSize = 10 * 1024 * 1024; // 예시: 10MB
        if (fileSize > maxFileSize) {
            response.setStatusCode(PayloadTooLarge_413);
            response.setHeader("Content-Type", "text/html; charset=utf-8");
            response.setHeader("Date", getCurTime());
            std::string errorBody = "<html><body><h1>413 Payload Too Large</h1><p>The requested file is too large.</p></body></html>";
            response.setBody(errorBody);
            response.setHeader("Content-Length", std::to_string(errorBody.length()));
            response.setHeader("Connection", "close");
            return response;
        }
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
        // 파일 내용 읽기
        std::string body(fileSize, '\0');
        file.read(&body[0], fileSize);

        response.setStatusCode(OK_200);
        response.setHeader("Date", getCurTime());
        response.setHeader("Content-Type", getContentType(extension));
        response.setBody(body);
        response.setHeader("Content-Length", std::to_string(body.length()));
        response.setHeader("Connection", "keep-alive");
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
        file.close();
    } else {
        std::cout << "File not found: " << filePath << std::endl;
        response.setStatusCode(NotFound_404);
        response.setHeader("Content-Type", "text/html; charset=utf-8");
        response.setHeader("Date", getCurTime());
        std::string errorBody = "<html><body><h1>404 Not Found</h1><p>The requested file was not found.</p></body></html>";
        response.setBody(errorBody);
        response.setHeader("Content-Length", std::to_string(errorBody.length()));
        response.setHeader("Connection", "close");
std::cout << "filePath: " << __LINE__ << filePath << std::endl;
    }

    response.setHeader("Server", "42Webserv");

    return response;
}

Response handleMethodNotAllowed() {
    Response response;
    response.setStatusCode(MethodNotAllowed_405);
    response.setHeader("Content-Type", "text/html");
    response.setHeader("Date", getCurTime());

    std::string errorBody = "<html><body><h1>405 Method Not Allowed</h1><p>The requested method is not allowed.</p></body></html>";
    response.setBody(errorBody);
    response.setHeader("Content-Length", std::to_string(errorBody.length()));
    response.setHeader("Connection", "close");

    return response;
}

Response Client::sendResponse(const Config &Conf) {
    enum Method {
        GET = 1,
        POST,
        PUT,
        DELETE
    };

    int method = getMethodNumber(_request._method);
    switch (method) {
        case GET:
            return handleGetRequest(Conf);
        case POST:
            // return handlePostRequest(Conf);
        case PUT:
            // return handlePutRequest(Conf);
        case DELETE:
            // return handleDeleteRequest(Conf);
        default:
            return handleMethodNotAllowed();
    }
}





std::string Client::execute(const Config &Conf) {
	Response response = sendResponse(Conf);
	std::string responseStr = response.get_responses();
	std::cout << responseStr << std::endl;
	_tempResult = responseStr;
	return responseStr;
}


/* auto index
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

std::string Client::handleAutoIndex(std::string servRoot)
{
    std::string locRoot = _request._uri;  // <-- uri를 어떻게 받을지만 정하면 됨. 임시적으로 클라이언트에 해서 편하게 받으려고만했던것.
    std::string dirPath = servRoot + locRoot;

    struct stat fileStat;
    std::stringstream body;
    body << "<html>\n<head>\n<title>Index of /" << locRoot << "</title>\n</head>\n<body>\n";
    body << "<h1>Index of " << locRoot << "</h1>\n";
	body << "<hr> <pre>\n<table>\n<tr><th></th><th></th><th></th></tr>\n";

    DIR *dir = opendir(dirPath.c_str());
	if (dir == NULL) {
		_responseStatus = 404;
		return "";
	}
    if (dir)
    {
        std::vector<std::string> fileList;
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL)
            fileList.push_back(ent->d_name);
        closedir(dir);

        std::sort(fileList.begin(), fileList.end());

        int count = fileList.size();
        for (int i = 0; i < count; i++)
        {
            std::string fileName = fileList[i];
            if (fileName == ".")
                continue;
            std::string filePath = dirPath + "/" + fileName;
            if (stat(filePath.c_str(), &fileStat) == 0)
            {
                body << "<tr>" << "<td>";
                if (S_ISDIR(fileStat.st_mode))
                    body << "<a href=\"" << fileName << "/\">" << fileName << "/</a>";
                else
                    body << "<a href=\"" << fileName << "\">" << fileName << "</a>";
                body << " </td> <td>\t\t" << FormatTime(fileStat.st_mtime) << "</td>";
                double fileSize = static_cast<double>(fileStat.st_size);
                body << "<td>\t\t" << FormatSize(fileSize) << " </td>" << "</tr>\n";
            }
        }
		body << " </table> </pre>\n<hr>\n</body>\n</html>\n";
    }

    return body.str();
}

*/