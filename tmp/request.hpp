#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <stdlib.h>
#include "client.hpp"

class HttpRequest {
	private:
		static void parseRequestLine(Request& req, const std::string& line);
		static void parseHeader(Request& req, const std::string& line);
		static void parseBody(const std::string &line, int& contentLength);
		static void initRequest(Request& req);
		static void isVaildRequest(const Request& req);
	public:
		static Request parse(const std::string& requestStr);
};

Request HttpRequest::parse(const std::string& requestStr)
{
	Request req;
	std::istringstream iss(requestStr);
	std::string line;
	std::string header;
	std::string body;

	try {

		std::getline(iss, line);
		parseRequestLine(req, line);

		std::string::size_type pos = iss.str().find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			header = iss.str().substr(0, pos);
			body = iss.str().substr(pos + 4);
		}
		else {
			throw std::invalid_argument("Invalid Request");
			// req._readStatus = READ_NOT_DONE;
			// return req;
		}

		std::istringstream headerStream(header);	
		while (std::getline(headerStream, line) && !line.empty()) {
			parseHeader(req, line);
		}

		if (req._headers.find("Content-Length") != req._headers.end()) {
			int contentLength = atoi(req._headers["Content-Length"].c_str());
			if (body.empty() || contentLength > body.length()) {
				req._readStatus = READ_NOT_DONE;
				return req;
			}
			std::istringstream bodyStream(body);
			while (getline(bodyStream, line) && !line.empty()) {
				if (contentLength == 0) {
					req._readStatus = READ_DONE;
					break;
				}
				parseBody(line, contentLength);
			}
			if (contentLength > 0) {
				initRequest(req);
				req._readStatus = READ_NOT_DONE;
				return req;
			}
		}
		isVaildRequest(req);
		req._readStatus = READ_DONE;
	}
	catch (std::invalid_argument& e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
		req._readStatus = READ_ERROR;
	}
	req.status = 200;
	return req;
}

void HttpRequest::initRequest(Request& req)
{
	req._method = "";
	req._uri = "";
	req._version = "";
	req._headers.clear();
	req._readStatus = READ_NOT_DONE;
	// req._status = 0;
}

void HttpRequest::isVaildRequest(const Request& req)
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

void HttpRequest::parseRequestLine(Request &req, const std::string& line)
{
    std::istringstream iss(line);
	std::string token;

	iss >> token;
	if (iss.fail())
		throw std::invalid_argument("Invalid Request Method");
	req._method = parseMethod(token);

	iss >> req._uri;
	if (iss.fail())
		throw std::invalid_argument("Invalid Request URI");
	iss >> req._version;
	if (iss.fail()) //req._version != "HTTP/1.1"
		throw std::invalid_argument("Invalid Request Version");
}

void HttpRequest::parseHeader(Request &req, const std::string& line)
{
    std::string::size_type pos = line.find(":");
    if (pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 2);
        req._headers[key] = value;
    }
}

void HttpRequest::parseBody(const std::string& line, int& contentLength)
{ //length가 양수인데 line이 비어있을 때 생각.
    std::istringstream iss(line);
    std::string token;
    while (std::getline(iss, token, '&')) {
        std::istringstream iss_token(token);
        std::string key, value;
		key = token.substr(0, token.find("="));
		value = token.substr(token.find("=") + 1);
		setenv(key.c_str(), value.c_str(), 1);
    }

    contentLength -= line.length();
}

#endif

//telnet을 사용하는 경우도 고려해야함. 즉, 유효성 검사를 해줘야함.
//

// favicon을 만나면 not found.

// 400은 요쳥이 대강 이상한경우
// 403은 권한없는거 (실행못하는 mothod나 등등)
// mothod가 없는것일 경우 413 err.
// F_OK 가 아닐경우 404 존재 x.
// F_OK이지만 open,execute 등등 권한이 없을경우 403


