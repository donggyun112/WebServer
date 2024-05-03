#include "Request.hpp"

void HttpRequest::isVaildRequest(const Request &req)
{
	if (req._method == "OTHER")
		throw std::invalid_argument("Invalid Method");
	if (req._uri.empty())
		throw std::invalid_argument("Invalid URI");
	if (req._version != "HTTP/1.1")
		throw std::invalid_argument("Invalid Version");
	if (req._headers.find("Host") == req._headers.end())
		throw std::invalid_argument("Invalid Host");
}

std::string parseMethod(const std::string& methodStr)
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

bool isQuary(const std::string &uri)
{
	if (uri.find('?') != std::string::npos)
		return true;
	return false;
}

std::string parseUri(const std::string& uri)
{
	std::string path, query;

	if (isQuary(uri)) {
			size_t pos = uri.find('?');
			path = uri.substr(0, uri.find('?'));
			query = uri.substr(uri.find('?') + 1);
			HttpRequest::parseQuery(query);
	}
	else 
		path = uri;
	return path;
}

void HttpRequest::parseRequestLine(Request &req, const std::string& line)
{
    std::istringstream iss(line);
	std::string token, tmpUri;

	iss >> token;
	if (iss.fail())
		throw std::invalid_argument("Invalid Request Method");
	req._method = parseMethod(token);

	iss >> tmpUri;
	if (iss.fail())
		throw std::invalid_argument("Invalid Request URI");
	req._uri = parseUri(tmpUri);

	iss >> req._version;
	if (iss.fail())
		throw std::invalid_argument("Invalid Request Version");
}

void HttpRequest::parseHeader(Request &req, const std::string& header)
{
	std::istringstream iss(header);
	std::string line;

	while (std::getline(iss, line))
	{
		std::string::size_type pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			req._headers[key] = value;
		}
	}
}

void HttpRequest::setCookie(Request &req)
{
	std::istringstream iss(req._headers["Cookie"]);
	std::string line;

	while (std::getline(iss, line, ';')) {
		std::string::size_type pos = line.find("=");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			req._cookie[key] = value;
		}
	}
}

void HttpRequest::parseQuery(const std::string& line)
{
	std::istringstream iss(line);
	std::string token;

	if (line.find("&") == std::string::npos) {
		std::string key = line.substr(0, line.find("="));
		std::string value = line.substr(line.find("=") + 1);
		setenv(key.c_str(), value.c_str(), 1);
		return ;
	}

	while (std::getline(iss, token, '&')) {
		std::istringstream iss_token(token);
		std::string key, value;
		key = token.substr(0, token.find("="));
		value = token.substr(token.find("=") + 1);
		setenv(key.c_str(), value.c_str(), 1);
	}
}

std::string HttpRequest::parseBody(const std::string& body)
{
	std::istringstream iss(body);
	std::string line;

	std::getline(iss, line);
	if (line.find('=') != std::string::npos && \
			line.find('&') != std::string::npos)
		parseQuery(line);
    return line;
}
