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

procInfo *Client::getProcInfo() const {
	return _procPtr;
}

void Client::setResponse(const std::string &param) {
	_response = param;
}

void Client::appendResponse(const char *param) {
	_response += param;
}

Port Client::getPort() const {
	return _port;
}

void Client::setBuffer(const std::string &buffer) {
	try {
		_requestHandle.setBuffer(buffer);
	} catch (StatusCode num) {
		this->_requestHandle.setReadStatus(READ_ERROR);
		_requestHandle.setResponseStatus(num);
	}
}

int Client::getReadStatus() const {
	return _requestHandle.getReadStatus();
}

bool Client::getIsKeepAlive() const {
	return _requestHandle.getIsKeepAlive();
}

void	Client::setEnv(const RequestHandle &Req) {
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
    	setenv("SERVER_PORT", serverPort.c_str(), 1);
        setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
        if (!cgiPath.empty())
            setenv("DOCUMENT_ROOT", cgiPath.c_str(), 1);
	    setenv("REQUEST_METHOD", requestMethod.c_str(), 1);
        setenv("SCRIPT_NAME", scriptName.c_str(), 1);
        setenv("REMOTE_HOST", host.c_str(), 1);
        if (!Req.getHeader("User-Agent").empty())
	        setenv("HTTP_USER_AGENT", Req.getHeader("User-Agent").c_str(), 1);
	    if (!Req.getHeader("Accept-Charset").empty())
            setenv("HTTP_ACCEPT_CHARSET", Req.getHeader("Accept-Charset").c_str(), 1);
	    if (!Req.getHeader("Accept-Language").empty())    
            setenv("HTTP_ACCEPT_LANGUAGE", Req.getHeader("Accept-Language").c_str(), 1);
	    if (!Req.getHeader("Accept-Encoding").empty())    
            setenv("HTTP_ACCEPT_ENCODING", Req.getHeader("Accept-Encoding").c_str(), 1);
        if (!Req.getHeader("Keep-Alive").empty())    
            setenv("HTTP_KEEP_ALIVE", Req.getHeader("Keep-Alive").c_str(), 1);
        if (!Req.getHeader("Connection").empty())
        	setenv("HTTP_CONNECTION", Req.getHeader("Connection").c_str(), 1);
        if (!Req.getHeader("Referer").empty())    
        	setenv("HTTP_REFERER", Req.getHeader("Referer").c_str(), 1);
        if (!Req.getHeader("Cookie").empty())    
            setenv("HTTP_COOKIE", Req.getHeader("Cookie").c_str(), 1);
        if (!Req.getHeader("Content-Type").empty())    
            setenv("HTTP_CONTENT_TYPE", Req.getHeader("Content-Type").c_str(), 1);
    }
    if (Req.getMethod() == "GET") {
        if (!_responseHandle.getPathInfo().empty())
    	    setenv("PATH_INFO", _responseHandle.getPathInfo().c_str(), 1);
        if (!query.empty())
	        setenv("QUERY_STRING", query.c_str(), 1);    
    }
    if (Req.getMethod() == "POST") {
        if (!Req.getHeader("Content-Length").empty())    
	        setenv("HTTP_CONTENT_LENGTH", Req.getHeader("Content-Length").c_str(), 1);
        // if (!Req.savePath.empty())
        //     setenv("SAVE_PATH", Req.savePath());
    }
}

void Client::makeTempFileNameForCgi(std::string &filePath) {
	std::srand(std::time(NULL));

	filePath = "/tmp/";
	filePath += "webserv-";
	filePath += Manager::utils.toString(std::rand());
}

void Client::handleCGI(char **env) {
	FD pipeParentToChild[2];
	int processPid;
	std::vector<char *> commands;
	std::string command = Manager::responseUtils.getFileExtension(_responseHandle.getFilePath());
	int tempFileFd;
	std::string filePath;
	makeTempFileNameForCgi(filePath);
	commands.push_back(const_cast<char*>(command.c_str()));
	commands.push_back(const_cast<char*>(_responseHandle.getFilePath().c_str()));
	commands.push_back(NULL);
	if (pipe(pipeParentToChild) == -1) {
		throw InternalServerError_500;
	} 
	processPid = fork();
	if (processPid == 0) {
		setEnv(_requestHandle);
		dup2(pipeParentToChild[READ], STDIN_FILENO);
		close(pipeParentToChild[WRITE]);
		tempFileFd = open(filePath.c_str(), O_RDWR | O_CREAT | O_TRUNC);
		if (tempFileFd == -1)
			exit(InternalServerError_500);
		dup2(tempFileFd, STDOUT_FILENO);
		if (execve(commands[0], commands.data(), env) == -1) {
			exit(InternalServerError_500);
		}
	} else if (processPid > 0) {
		close(pipeParentToChild[READ]);
		write(pipeParentToChild[WRITE], _requestHandle.getBody().c_str(), _requestHandle.getBody().length());
		close(pipeParentToChild[WRITE]);
		this->_procPtr = new procInfo();
		_procPtr->pid = processPid;
		_procPtr->tempFilePath = filePath;
		return ;
	} else {
		close(pipeParentToChild[READ]);
		close(pipeParentToChild[WRITE]);
		throw InternalServerError_500;
	}
}


void Client::generateResponse(const Config &Conf, char **env) {
	try {
		if (_requestHandle.getReadStatus() == READ_ERROR) {
			throw _requestHandle.getResponseStatus();
		}
		_responseHandle.initPathFromLocation(_requestHandle, Conf);
		if (_responseHandle.isCGI()) {
			handleCGI(env);
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
