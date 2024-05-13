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
	_requestHandle.setBuffer(buffer);
}

int Client::getReadStatus() const {
	return _requestHandle.getReadStatus();
}



void Client::generateResponse(Config Conf) {
	try {
		_response = _responseHandle.generateHTTPFullString(_requestHandle, Conf);
	}
	catch (int num) {
		_response = Error::errorHandler(Conf[_port], num);
	}
	clearAll();
}

std::string Client::getResponse() const {
	return _response;
}