#include "Client.hpp"

Client::Client(Port port) : _port(port), _requestHandle(port) {};

Client::Client(const Client &Copy) : _port(Copy._port), _requestHandle(Copy._requestHandle) {};

void Client::clearAll() {
	_requestHandle.clearAll();
	_responseHandle.clearAll();
}

Client::~Client() {
	clearAll();
}


void Client::setBuffer(const std::string &buffer) {
	try {
		std::cout << "jackkkkkkk jacque" << std::endl;
		_requestHandle.setBuffer(buffer);
	} catch (StatusCode num) {
		this->_requestHandle.setReadStatus(READ_ERROR);
		_requestHandle.setResponseStatus(num);
	}
}

int Client::getReadStatus() const {
	return _requestHandle.getReadStatus();
}



void Client::generateResponse(Config Conf) {
	try {
		if (_requestHandle.getReadStatus() == READ_ERROR) {
			throw _requestHandle.getResponseStatus();
		}
		_responseHandle.initPathFromLocation(_requestHandle, Conf);
		if (_responseHandle.isCGI()) {
			
		} else {
			_response = _responseHandle.generateHTTPFullString(_requestHandle, Conf);
		}
	} catch (int num) {
		_response = Error::errorHandler(Conf[_port], num);
	} catch (StatusCode num) {
		_response = Error::errorHandler(Conf[_port], num);
		// std::cout << __LINE__ << "      :" << _response << std::endl;
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	clearAll();
}

std::string Client::getResponse() const {
	return _response;
}