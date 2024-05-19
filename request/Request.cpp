#include "Request.hpp"

RequestUtils::RequestUtils() {}
RequestUtils::~RequestUtils() {}

bool RequestUtils::isQuary(const std::string &uri)
{
	if (uri.find('?') != std::string::npos)
		return true;
	return false;
}

std::string RequestUtils::parseMethod(const std::string& methodStr)
{
	if (methodStr == "GET")
		return "GET";
	else if (methodStr == "POST")
		return "POST";
	else if (methodStr == "DELETE")
		return "DELETE";
	else
		return "OTHER";
}
