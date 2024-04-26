#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <cctype>
#include <stdexcept>
#include <stdlib.h>
#include "structRq.hpp"
#include "Client.hpp"

class Client;

class HttpRequest {
	public:
		static void parseRequestLine(Request& req, const std::string& line);
		static void parseHeader(Request& req, const std::string& line);
		static std::string parseBody(const std::string &body);
		static void isVaildRequest(const Request &req);
		static void	setCookie(Request &req);
		static void parseQuery(const std::string& line);
};

#endif