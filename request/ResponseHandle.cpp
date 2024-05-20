#include "ResponseHandle.hpp"
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "NResponseUtils.hpp"
#include "../Manager/Manager.hpp"

ResponseHandle::ResponseHandle() : _isInitFromLocation(false) {
}

ResponseHandle::ResponseHandle(const ResponseHandle &Copy) : _response(Copy._response) {}

ResponseHandle::~ResponseHandle() {}


bool ResponseHandle::isCGI() const {
	return _loc.isCgi();
}

void	ResponseHandle::clearAll() {
	_response.clear();
	_filePath.clear();
	_pathInfo.clear();
	_scriptName.clear();
	_httpUri.clear();
	// _serverRoot.clear();
}

std::string ResponseHandle::generateHTTPFullString(const RequestHandle &Req, const Config &Conf)
{
	//
	if (_isInitFromLocation == false) {
		initPathFromLocation(Req, Conf);
	}
	//
	// std::cout << "Start to generate response" << std::endl;
	int method = Manager::responseUtils.getMethodNumber(Req.getMethod());
	// std::cout << "method number = " << method << "string = " << Req.getMethod() << "123" << std::endl;
	switch (method)
	{
	case GET:
		// std::cout << "Goto GET" << std::endl;
		_response = handleGetRequest(Req);
		return _response;
	case POST:
		std::cout << "Goto POST" << std::endl;
		_response = handlePostRequest(Req); // -> 1. Client받아서 setRespose
		// std::cout << "----------------\n"
		// 		  << Req.getBody() << "\n----------------" << std::endl;
		break;
	case DELETE:
		// _response = handleDeleteRequest();
		std::cout << "Goto DELETE" << std::endl;
		break;
	default:
		// _response = createErrorResponse(MethodNotAllowed_405, "The requested method is not allowed.");
		std::cout << "Goto ERROR" << std::endl;
		return handleMethodNotAllowed().getResponses();
		break;
	}
	return "";
}

const std::string ResponseHandle::getResponse()
{
	if (_response.empty())
	{
		throw std::invalid_argument("Response is empty");
	}
	return _response;
}

void ResponseHandle::setResponse(const std::string &response)
{
	_response = response;
}

Response ResponseHandle::handleMethodNotAllowed()
{
	Response response;
	response.setStatusCode(MethodNotAllowed_405);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Date", Manager::responseUtils.getCurTime());

	std::string errorBody = "<html><body><h1>405 Method Not Allowed</h1><p>The requested method is not allowed.</p></body></html>";
	response.setBody(errorBody);
	response.setHeader("Content-Length", Manager::utils.toString(errorBody.length()));
	response.setHeader("Connection", "close");

	return response;
}





std::string ResponseHandle::getFilePath(const std::string &serverRoot, const std::string &httpUri, LocationConfig &loc) {
    std::string filePath;
    std::string alias = loc.getAlias();


    if (!alias.empty() && httpUri.find(alias) == 0) {
        filePath = alias + httpUri.substr(alias.length());
        std::cout << "1" << std::endl;
    }

	if (loc.isCgi()) {
		if (httpUri.substr(0, httpUri.find_last_of('/')).find('.') != std::string::npos) {
			_scriptName = httpUri.substr(0, httpUri.find_last_of('/'));
			_pathInfo = httpUri.substr(httpUri.find_last_of('/'));
			_httpUri = _scriptName;
		} else {
			_scriptName = httpUri;
			_pathInfo = "";
			_httpUri = _scriptName;
		}
	}
	// else


	if (httpUri.find(loc.getPath()) == std::string::npos || loc.getPath().find('.') != std::string::npos) {
		std::cout << "1" << std::endl;
		std::cout << "loc.getPath() : " << loc.getPath() << std::endl;
		std::cout << "httpUri : " << httpUri << std::endl;
		if (loc.isCgi() == true) {
			filePath = serverRoot + loc.getFastcgiPass() + httpUri.substr(httpUri.find_last_of('/'));
		} else if (loc.getPath().find('.') != std::string::npos) {
			filePath = serverRoot + loc.getRoot() + httpUri.substr(httpUri.find_last_of('/'));
		}else {
			filePath = serverRoot + loc.getRoot() + httpUri;
		}
	} else {
		// std::cout << "2" << std::endl;
		if (loc.isCgi() == true) {
			filePath = serverRoot + loc.getFastcgiPass() + httpUri.substr(loc.getPath().length(), httpUri.length() - loc.getPath().length());
		} else {
			// std::cout << "HTTP URI : " << httpUri << std::endl;
			std::cout << httpUri.substr(loc.getPath().length(), httpUri.length() - loc.getPath().length()) << std::endl;
			// std::cout << "ROOT : " << loc.getPath() << std::endl;
			filePath = serverRoot + loc.getRoot() + "/" + httpUri.substr(loc.getPath().length(), httpUri.length() - loc.getPath().length());
		}
	}
	filePath = Manager::responseUtils.normalizePath(filePath);
	if (Manager::responseUtils.isDirectory(filePath) && filePath[filePath.length() - 1] != '/') {
		filePath += "/";
	}
	
    return filePath;
}



Response ResponseHandle::handleRedirect(const LocationConfig &location)
{
	Response response;
	std::string returnCode = location.getReturnCode();
	std::string returnUrl = location.getReturnUrl();

	if (!returnCode.empty() && !returnUrl.empty())
	{
		std::cout << "Redirected to: " << returnUrl << std::endl;
		std::cout << "Status code : " << returnCode << std::endl;
		int statusCode = std::stoi(returnCode);
		response.setRedirect(returnUrl, statusCode);
		
		// 캐쉬 무효화
		response.setHeader("Cache-Control", "no-cache, no-store, must-revalidate");
		response.setHeader("Pragma", "no-cache");
		response.setHeader("Expires", "0");

		response.setHeader("Connection", "close");
		response.setBody("42Webserv Redirected");
		std::cout << response.getResponses() << std::endl;
		return response;
	}
	response.setStatusCode(OK_200);
	return response;
}

bool	ResponseHandle::initPathFromLocation(const RequestHandle &Req, const Config &Conf) {
	_isInitFromLocation = true;
	_httpUri = Req.getUri();
	_port = Req.getPort();
	// Conf.setServerName(Req.getHost());

	// URL 정규화
	_httpUri = Manager::responseUtils.nomralizeUrl(_httpUri);
	// std::cout << "Normalized URL: " << _httpUri << std::endl;
	_serverRoot = Manager::responseUtils.normalizePath(Conf.getServerConfig(_port, Req.getHost()).getPath());
	if (_serverRoot.empty())
	{
		throw InternalServerError_500;
		// _response = createErrorResponse(InternalServerError_500, "Server configuration error: root directory not set.");
	}

		_loc = Conf.getServerConfig(_port, Req.getHost()).getLocation(_httpUri);
		// std::cout << "Location Path: " << _loc.getPath() << std::endl;
		// std::cout << "Success to get location "<< _loc.getPath() << std::endl;
	if (Manager::responseUtils.isMethodPossible(GET, _loc) == false) {
		throw MethodNotAllowed_405;
		// _response = createErrorResponse(MethodNotAllowed_405, "The requested method is not allowed.");
	}
	_filePath = getFilePath(_serverRoot, _httpUri, _loc);
	if (!Manager::responseUtils.isValidPath(_filePath)) {
		throw BadRequest_400;
		// _response = createErrorResponse(BadRequest_400, "Invalid request path.");
	}

	if (_filePath.length() > 1 && _filePath[_filePath.length() - 1] == '/') {
		if (Manager::responseUtils.isDirectory(_filePath) == true) {
			std::string tmpPath = _filePath + _loc.getIndex();
			if (Manager::responseUtils.isExist(_filePath) == true) {
				_filePath = tmpPath;
			}
		} else {
			throw NotFound_404;
		}
	} else {
		if (Manager::responseUtils.isExist(_filePath) == false) {
			_filePath = _filePath.substr(0, _filePath.find_last_of('/') + 1);
		}
	}
	std::cout << "----------------\n"
			  << _filePath << "\n----------------" << std::endl;
			  
	// std::cout << "File Path: " << _filePath << std::endl;
	return true;
}



std::string ResponseHandle::handleGetRequest(const RequestHandle &Req)
{
	Response response;
	if (_loc.isEtag() == true && (Req.getHeader("If-Modified-Since") == Manager::responseUtils.lastModify(_filePath) || Req.getHeader("If-None-Match") == Manager::responseUtils.etag(_filePath)))
	{
		std::cerr << "Not Modified" << std::endl;
		response.setStatusCode(NotModified_304);
		response.setHeader("Content-Length", "0");
		response.setHeader("Date", Manager::responseUtils.getCurTime());
		response.setHeader("Server", "42Webserv");
		response.setHeader("Connection", "close");

		return response.getResponses();
	}
	if (_loc.isCgi() == true)
	{
		// CGI 처리
		(void)Req;
		// setEnv(Req);
		// printAllEnv();

		std::cout << "Start to handle CGI" << std::endl;
		// response = handleCgi(_loc, _filePath);
	}


	// Req.getHeader("")
	std::cout << Req.getBuffer() << std::endl;

	// std::cout << "Start to handle GET request" << std::endl;
	// 리다이렉트 처리

	Response redirectResponse = handleRedirect(_loc);
	// std::cout << "RE\n";
	if (redirectResponse.getStatusCode() != OK_200) {
		std::cout << "what the fuck" << std::endl;
		return redirectResponse.getResponses();
	}

	// 인덱스 파일 설정
	// std::cout << "Start to get file && isDirestory : " << Manager::responseUtils.isDirectory(_filePath) << std::endl;


	// 파일 확장자 추출
	// std::cout << "Start to get file extension" << std::endl;
	std::string extension = Manager::responseUtils.getFileExtension(_filePath);
	// 파일 읽기
	std::ifstream file(_filePath.c_str(), std::ios::binary);
	// std::cout << "File Path: " << _filePath << std::endl;

    if (file.is_open() && file.good() && Manager::responseUtils.isDirectory(_filePath) == false) {
        // 파일 크기 확인
        std::streamsize fileSize = Manager::responseUtils.getFileSize(file);

		// 파일 크기 제한 설정
		const std::streamsize maxFileSize = 10 * 1024 * 1024;
		if (fileSize > maxFileSize)
		{
			throw PayloadTooLarge_413;
		}
		// 파일 내용 읽기
		std::string body = Manager::responseUtils.readFileContent(file, fileSize);
		file.close();

		response.setStatusCode(OK_200);
		response.setHeader("Date", Manager::responseUtils.getCurTime());
		response.setHeader("Content-Type", Manager::responseUtils.getContentType(extension));
		response.setBody(body);
		response.setHeader("Content-Length", Manager::utils.toString(body.length()));
		if (_loc.isEtag() == true)
		{
			std::cerr << "ETAG" << std::endl;
			response.setHeader("Last-Modified", Manager::responseUtils.lastModify(_filePath));
			response.setHeader("ETag", Manager::responseUtils.etag(_filePath));
			response.setHeader("Cache-Control", "max-age=3600, public, must-revalidate, no-cache");
			response.setHeader("Expires", Manager::responseUtils.getExpirationTime(3600)); // 1시간 후 만료
		} else {
			response.setHeader("Cache-Control", "no-cache, no-store, must-revalidate");
			response.setHeader("Pragma", "no-cache");
			response.setHeader("Expires", "0");
		}

	}
	else
	{
		if (Manager::responseUtils.isDirectory(_filePath))
		{
			if (_loc.getAutoindex() == true)
			{
				handleAutoIndex(response, _filePath);
			}
			else
			{
				throw Forbidden_403;
			}
		}
		else
		{
			if (_loc.getAutoindex() == false)
			{
				throw NotFound_404;
			}
			else
			{
				handleAutoIndex(response, _filePath.substr(0, _filePath.find_last_of('/')));
			}
		}
	}
	response.setHeader("Server", "42Webserv");
	return response.getResponses();
}

std::string ResponseHandle::handlePostRequest(const RequestHandle &Req)
{
	std::string responseData;

	std::string contentType = Req.getHeader("Content-Type");
	if ((contentType.find("multipart/form-data") != std::string::npos && contentType.find("boundary") != std::string::npos) \
		|| contentType.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		if (!Req.getBody().empty())
		{
			// responseData = handleFormData(_filePath, Req);

			if (responseData.empty())
				throw InternalServerError_500;
		}
		else {
			throw BadRequest_400;
		}
	}
	else
		throw InternalServerError_500;
	return responseData;
}

// std::string ResponseHandle::handleFormData(const std::string &cgiPath, const RequestHandle &Req) {
//     int cgiInput[2];
//     pid_t pid;

//     if (pipe(cgiInput) < 0)
//         return "";

//     if ((pid = fork()) < 0)
//         return "";

//     if (pid == 0) {
//         setEnv(Req);
//         close(cgiInput[0]);
//         dup2(cgiInput[1], 1);
//         close(cgiInput[1]);

//         // std::string pythonPath = "/Library/Frameworks/Python.framework/Versions/3.11/lib/python3.11/site-packages";
//         // std::string pythonEnv = "PYTHONPATH=" + pythonPath;
//         // char* envp[] = {(char*)pythonEnv.c_str(), NULL};
//         // 혹시 몰라 일단 남겨둠 -> python 실행 해보고 필요해지면 추가 할 예정

//         std::string py3 = "/usr/bin/python3"; //
//         char* const arg[] = {(char *)py3.c_str(), (char *)cgiPath.c_str(), NULL};

//         if (execve(py3.c_str(), arg, NULL) == -1) {
//             perror("execve failed");
//             // exit(404);
//         }
//     } else {
//         int status;
//         close(cgiInput[1]);
//         waitpid(pid, &status, 0);
//         // if (status == 404)
//         //     return "";

//         std::string output;
//         char buf[1024];
//         ssize_t bytesRead;
//         while ((bytesRead = read(cgiInput[0], buf, sizeof(buf))) > 0) {
//             output.append(buf, bytesRead);
//         } // --> nonblocking
//         close(cgiInput[0]);
//         return output;
//     }
//     return "";
// }



void ResponseHandle::handleAutoIndex(Response &response, const std::string &servRoot)
{
	std::string dirPath = servRoot;

	struct stat fileStat;
	std::stringstream body;
	body << "<html>\n<head>\n<title> AutoIndex </title>\n";
	body << "<style>\n"
		<< "th, td {\n"
		<< "    padding-left: 10px;\n"
		<< "    padding-right: 50px;\n"
		<< "}\n"
		<< "</style>\n"
		<< "</head>\n<body>\n";
	body << "<h1>Index of / </h1>\n";
	body << "<hr> <pre>\n<table>\n<tr><th></th><th></th><th></th></tr>\n";

	DIR *dir = opendir(dirPath.c_str());
	if (dir == NULL && errno == EACCES)
		throw Forbidden_403;
	else if (dir == NULL)
		throw NotFound_404;
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
			if (stat(filePath.c_str(), &fileStat) == -1)
				throw ServiceUnavailable_503;
			if (stat(filePath.c_str(), &fileStat) == 0)
			{
				body << "<tr>" << "<td>";
				if (S_ISDIR(fileStat.st_mode))
					body << "<a href=\"" << fileName << "/\">" << fileName + "/" << "</a>";
				else
					body << fileName;
				body << "</td><td>\t\t" << Manager::responseUtils.getFormattedTime(fileStat.st_mtime) << "</td>";
				double fileSize = static_cast<double>(fileStat.st_size);
				body << "<td>\t\t" << Manager::responseUtils.getFormatSize(fileSize) << "</td>" << "</tr>\n";
			}
			else
				throw InternalServerError_500;
		}
		body << " </table> </pre>\n<hr>\n</body>\n</html>\n";
		response.setStatusCode(OK_200);
		response.setHeader("Date", Manager::responseUtils.getCurTime());
		response.setHeader("Content-Type", "text/html");
		response.setBody(body.str());
		response.setHeader("Content-Length", Manager::utils.toString(body.str().length()));
		response.setHeader("Connection", "keep-alive");
	}
}

void printAllEnv()
{
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
	std::cout << "HTTP_CONTENT_TYPE: " << getenv("HTTP_CONTENT_TYPE") << std::endl;
	std::cout << "HTTP_CONTENT_LENGTH: " << getenv("HTTP_CONTENT_LENGTH") << std::endl;
}

std::string	ResponseHandle::getFilePath() const {
	return _filePath;
}

std::string	ResponseHandle::getPathInfo() const {
	return _pathInfo;
}

std::string	ResponseHandle::getScriptName() const {
	return _scriptName;
}

std::string	ResponseHandle::getHttpUri() const {
	return _httpUri;
}

std::string	ResponseHandle::getServerRoot() const {
	return _serverRoot;
}

Port	ResponseHandle::getPort() const {
	return _port;
}

LocationConfig	ResponseHandle::getLocation() const {
	return _loc;
}

ResponseHandle&	ResponseHandle::operator=(const ResponseHandle &Copy) {
	if (this == &Copy)
		return *this;
	_response = Copy._response;
	_isInitFromLocation = Copy._isInitFromLocation;
	_filePath = Copy._filePath;
	_pathInfo = Copy._pathInfo;
	_scriptName = Copy._scriptName;
	_httpUri = Copy._httpUri;
	_serverRoot = Copy._serverRoot;
	_port = Copy._port;
	_loc = Copy._loc;
	return *this;
}

