#include "Client.hpp"

Client::Client(Port port) : _port(port), _requestHandle (port), _procPtr(NULL)
{};

Client::Client(const Client &Copy) : _port(Copy._port), _requestHandle(Copy._requestHandle) {};

void Client::clearAll() {
	_requestHandle.clearAll();
	_responseHandle.clearAll();
}

Client::~Client() {
	clearAll();
}

const ResponseHandle& Client::getResponseHandle() const{
	return _responseHandle;
}

const RequestHandle& Client::getRequestHandle() const {
	return _requestHandle;
}

procInfo *Client::getProcInfo() const {
	return _procPtr;
}

void Client::setResponse(const std::string &param) {
	_response = param;
}

void Client::appendResponse(const char *param) {
	if (param)
		_response += param;
	// _response += param;
}

Port Client::getPort() const {
	return _port;
}


void Client::cutRequestBody(size_t length) {
    _requestBody.erase(_requestBody.begin(), _requestBody.begin() + length);
}

std::string	Client::getRequestBody() {
	return _requestBody;
}

void Client::setBuffer(unsigned char *buffer, int length) {
	try {
		_requestHandle.setBuffer(buffer, length);
		if (_requestHandle.getReadStatus() == READ_DONE) {
			this->_requestBody = _requestHandle.getBody();
		}
	} catch (StatusCode num) {
		this->_requestHandle.setReadStatus(READ_ERROR);
		_requestHandle.setResponseStatus(num);
	}
}

int		Client::getReadStatus() const {
	return _requestHandle.getReadStatus();
}

bool	Client::getIsKeepAlive() const {
	return _requestHandle.getIsKeepAlive();
}

void	Client::makeExecuteCommand(std::string &extention) {
	std::cout << __LINE__ << " | extention = " << extention << std::endl;
    if (extention == "py") {
        extention = "/usr/bin/python3";
    } else if (extention == "php") {
        extention = "/usr/bin/php";
    } else if (extention == "js") {
        extention = "/usr/bin/node";
    } else if (extention == "rb") {
        extention = "/usr/bin/ruby";
    } else if (extention == "pl") {
        extention = "perl";
	}
}

void	Client::setEnv(const Config &Conf, const RequestHandle &Req) {
	std::string host = Req.getHost();
	std::string uri = _responseHandle.getHttpUri();
	std::string scriptName = _responseHandle.getScriptName();
	std::string query = Req.getQuery();
	std::string queryString = "";
	std::string requestUri = Req.getUri();
	std::string requestMethod = Req.getMethod();
	std::string serverName = host;
	std::string cgiPath = _responseHandle.getFilePath();
	std::string serverPort = Manager::utils.toString(_port);
    if (Req.getMethod() == "GET" || Req.getMethod() == "POST") {
        setenv("SERVER_SOFTWARE", "webserv", 1);
    	setenv("SERVER_NAME", serverName.c_str(), 1);
        setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    	setenv("SERVER_PORT", serverPort.c_str(), 1);
		setenv("SERVER_ADMIN", "", 1);
		setenv("DOCUMENT_ROOT", Conf[_port].getPath().c_str(), 1);
	    setenv("REQUEST_METHOD", requestMethod.c_str(), 1);
        setenv("REMOTE_HOST", host.c_str(), 1);
		setenv("REMOTE_ADDR", "", 1);
		setenv("REMOTE_USER", "", 1);
        setenv("SCRIPT_NAME", scriptName.c_str(), 1);
        setenv("HTTP_ACCEPT_CHARSET", Req.getHeader("Accept-Charset").c_str(), 1);
        setenv("HTTP_ACCEPT_LANGUAGE", Req.getHeader("Accept-Language").c_str(), 1);
    	setenv("HTTP_CONNECTION", Req.getHeader("Connection").c_str(), 1);
        setenv("HTTP_COOKIE", Req.getHeader("Cookie").c_str(), 1);
        setenv("HTTP_HOST", host.c_str(), 1);
    	setenv("HTTP_REFERER", Req.getHeader("Referer").c_str(), 1);
	    setenv("HTTP_USER_AGENT", Req.getHeader("User-Agent").c_str(), 1);
		setenv("HTTP_PRAGMA", Req.getHeader("Cache-Control").c_str(), 1);
        setenv("HTTP_ACCEPT_ENCODING", Req.getHeader("Accept-Encoding").c_str(), 1);
        setenv("HTTP_KEEP_ALIVE", Req.getHeader("Keep-Alive").c_str(), 1);
    }
    if (Req.getMethod() == "GET") {
    	setenv("PATH_INFO", _responseHandle.getPathInfo().c_str(), 1);
	    setenv("QUERY_STRING", query.c_str(), 1);    
    }
    if (Req.getMethod() == "POST") {
	    setenv("CONTENT_LENGTH", Req.getHeader("Content-Length").c_str(), 1);
        setenv("CONTENT_TYPE", Req.getHeader("Content-Type").c_str(), 1);
    }
}
void Client::makeTempFileNameForCgi(std::string &filePath) {
	std::srand(std::time(NULL));

	filePath = "/tmp/webserv-";
	filePath += Manager::utils.toString(std::rand());
}

void Client::handleCGI(const Config &Conf) {
	int processPid, CgiOutputFd, CgiInputFd;
	std::vector<char *> commands;
	std::string serviceName = Manager::responseUtils.getFileExtension(_responseHandle.getFilePath());
	std::string pathToExecute = _responseHandle.getFilePath();
	std::string OutputFileName, InputFileName;

	makeExecuteCommand(serviceName);
	std::cout << __LINE__ << " | serviceName after change= " << serviceName << std::endl;
	makeTempFileNameForCgi(OutputFileName);
	makeTempFileNameForCgi(InputFileName);
	commands.push_back(const_cast<char*>(serviceName.c_str()));
	commands.push_back(const_cast<char*>(pathToExecute.c_str()));
	commands.push_back(NULL);
	CgiOutputFd = open(OutputFileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
	CgiInputFd = open(InputFileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
	Socket::nonblocking(CgiOutputFd);
	Socket::nonblocking(CgiInputFd);
	std::remove(OutputFileName.c_str());
	std::remove(InputFileName.c_str());
	processPid = fork();
	if (processPid == 0) {
		setEnv(Conf, _requestHandle);
		dup2(CgiInputFd, STDIN_FILENO);
		extern char **environ;
		dup2(CgiOutputFd, STDOUT_FILENO);
		if (execve(commands[0], commands.data(), environ) == -1) {
			std::cerr << "failed to execve | " << __LINE__ << std::endl;
			exit(InternalServerError_500);
		}
	} else if (processPid > 0) {
		// write(CgiInputFd, _requestHandle.getBody().data(), _requestHandle.getBody().length());
		this->_procPtr = new procInfo();
		_procPtr->pid = processPid;
		_procPtr->CgiInputFd = CgiInputFd;
		_procPtr->CgiOutputFd = CgiOutputFd;
		std::cout << "process parent action success" << std::endl;
		return ;
	} else {
		std::cout << "failed to fork()" << std::endl;
		throw InternalServerError_500;
	}
}

void Client::generateResponse(const Config &Conf) {
	try {
		if (_requestHandle.getReadStatus() == READ_ERROR) {
			throw _requestHandle.getResponseStatus();
		}
		_responseHandle.initPathFromLocation(_requestHandle, Conf);
		if (_responseHandle.isCGI()) {
			handleCGI(Conf);
			return ;		
		} else {
			_response = _responseHandle.generateHTTPFullString(_requestHandle, Conf);
		}
	} catch (int num) {
		_response = Error::errorHandler(Conf[_port], num);
	} catch (StatusCode num) {
		_requestHandle.setReadStatus(READ_ERROR);
		// _responseHandle.getLocation().setCgi(false);
		_response = Error::errorHandler(Conf[_port], num);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

std::string Client::getResponse() const {
	return _response;
}

void	Client::cutResponse(int length) {
	_response.erase(_response.begin(), _response.begin() + length);
}
