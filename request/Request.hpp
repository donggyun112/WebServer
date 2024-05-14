#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <cctype>
#include <stdexcept>
#include <stdlib.h>
#include "structRq.hpp"
#include <sstream>
#include "../utils/utils.hpp"

class RequestHandle;
struct Request;

namespace HttpRequest {
	void parseRequestLine(Request& req, const std::string& line);
	void parseHeader(Request& req, const std::string& line);
	void validateRequest(const Request &req);
	void setCookie(Request &req);
	std::string parsePart(const std::string& body, const std::string& boundary);
	std::string parseFileContent(const std::string &body);
	std::string parseBodyHeader(const std::string& part);
	std::string parseType(const std::string& body_header);
	std::string parseFileName(const std::string& body_header);
	std::string parseBoundary(const std::string& body_header);
	std::string parseContentType(std::string &body_header);
};

#endif