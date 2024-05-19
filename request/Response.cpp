#include "Response.hpp"

Response::Response() : _version("HTTP/1.1"), _status(-1) {}


void Response::setVersion(const std::string& version) {
	_version = version;
}

std::string Response::getVersion() const {
	return _version;
}

int Response::getStatusCode() const {
	return _status;
}

std::string Response::getReason() const {
	return _reason;
}

void Response::setHeader(const std::string& name, const std::string& value) {
	headers.insert(std::make_pair(name, value));
}

void Response::setRedirect(const std::string& url, int status) {
	setHeader("Location", url);
	setStatusCode(status);
}

void Response::setStatusCode(int status) {
	_reason = Error::errors[status];
	this->_status = status;
}

void Response::removeHeader(const std::string& name) {
	std::pair<Headers::iterator, Headers::iterator> range = headers.equal_range(name);
	headers.erase(range.first, range.second);
}

std::string Response::getHeaderValue(const std::string& name) const {
	std::pair<Headers::const_iterator, Headers::const_iterator> range = headers.equal_range(name);
	if (range.first != headers.end()) {
		return range.first->second;
	}
	return std::string() = "";
}

std::vector<std::string> Response::getHeaderValues(const std::string& name) const {
	std::vector<std::string> values;
	std::pair<Headers::const_iterator, Headers::const_iterator> range = headers.equal_range(name);
	for (Headers::const_iterator it = range.first; it != range.second; ++it) {
		values.push_back(it->second);
	}
	return values;
}

bool Response::hasHeader(const std::string& name) const {
	return headers.find(name) != headers.end();
}

void Response::setBody(const std::string& body) {
	this->_body = body;
}

const std::string Response::getResponses() {
	std::string response = _version + " " +  Manager::utils.toString(_status) + " " + Error::errors[_status] + "\r\n";
	for (std::multimap<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n" + _body;
	return response;	
}

Response &Response::operator=(const Response &Copy) {
	_version = Copy._version;
	_status = Copy._status;
	_reason = Copy._reason;
	headers = Copy.headers;
	_body = Copy._body;
	return *this;
}

void Response::clearAll() {
	_version.clear();
	_status = -1;
	_reason.clear();
	headers.clear();
	_body.clear();
}


Response::~Response() {
	clearAll();
}