#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <cctype>
#include <stdexcept>
#include <stdlib.h>
#include "structRq.hpp"
#include <sstream>

class RequestHandle;
struct Request;

namespace HttpRequest {
	static void parseRequestLine(Request& req, const std::string& line);
	static void parseHeader(Request& req, const std::string& line);
	static void isVaildRequest(const Request &req);
	static void	setCookie(Request &req);
	static void std::string parsePart(const std::string& body, const std::string& boundary);
	static std::string parseFileContent(const std::string &body);
	static std::string parseBodyHeader(const std::string& part);
	static std::string parseType(const std::string& body_header);
	static std::string parseFileName(const std::string& body_header);
	static std::string parseBoundary(const std::string& body_header);
	static std::string parseContentType(std::string &body_header);
};

#endif