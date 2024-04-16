#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <algorithm>

enum Method { GET, POST, DELETE, OTHER };

class HttpRequest {
public:
    void parse(const std::string& request);
    const std::string& getMethod() const;
    const std::string& getUri() const;
    const std::string& getVersion() const;
    const std::map<std::string, std::string>& getHeaders() const;
    const std::string& getBody() const;

private:
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    void parseRequestLine(const std::string& line);
    void parseHeader(const std::string& line);
    std::string parseMethod(const std::string& methodStr);
};

void HttpRequest::parse(const std::string& request)
{
	std::istringstream iss(request);
	std::string line;

	std::getline(iss, line);
	parseRequestLine(line);

	while (std::getline(iss, line) && !line.empty()) {
		parseHeader(line);
	}

	if (headers.find("Content-Length") != headers.end()) {
		int contentLength = atoi(headers["Content-Length"].c_str());
		std::string body(contentLength, '\0');
		iss.read(&body[0], contentLength);
		this->body = body;
	}
}

const std::string& HttpRequest::getMethod() const { return method; }

const std::string& HttpRequest::getUri() const { return uri; }

const std::string& HttpRequest::getVersion() const { return version; }

const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return headers; }

const std::string& HttpRequest::getBody() const { return body; }

void HttpRequest::parseRequestLine(const std::string& line)
{
    std::istringstream iss(line);
	std::string token;

	iss >> token;
	method = parseMethod(token);

	iss >> uri;

	iss >> version;
}

std::string HttpRequest::parseMethod(const std::string& methodStr)
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

void HttpRequest::parseHeader(const std::string& line)
{
	std::string::size_type pos = line.find(":");
	if (pos != std::string::npos) {
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 2);
		headers[key] = value;
	}
}


#endif