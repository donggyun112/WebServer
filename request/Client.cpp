#include "Client.hpp"

Client::Client(Port port) : _port(port), _requestHandle(port) {};

Client::Client(const Client &Copy) : _port(Copy._port), _requestHandle(Copy._requestHandle) {};

void Client::clearAll() {
	_requestHandle.clearAll();
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

void Client::executeRequest(const Config &Conf) {
	_response = _requestHandle.execute(Conf);
}

const std::string Client::getResponse() const {
	return _response;
}