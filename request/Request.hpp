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

class RequestUtils {
	public:
	RequestUtils();
	~RequestUtils();
	bool isQuary(const std::string &uri);
	std::string parseMethod(const std::string& methodStr);
	
};

#endif