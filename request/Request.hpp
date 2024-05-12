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

class HttpRequest {
	public:
		static void parseRequestLine(Request& req, const std::string& line);
		static void parseHeader(Request& req, const std::string& line);
		static void isVaildRequest(const Request &req);
		static void	setCookie(Request &req);
 };

#endif