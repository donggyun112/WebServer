#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include "client.hpp"

class HttpRequest {
	private:
		static void parseRequestLine(Request& req, const std::string& line);
		static void parseHeader(Request& req, const std::string& line);
		// static void parseBody(const std::string &body);
		static void setBodyEnv();
		static void initRequest(Request& req);
		static void isVaildRequest(const Request& req);
	public:
		static Request parse(const std::string& requestStr);
};

Request HttpRequest::parse(const std::string& requestStr) //body length, 및 status, /r/n/r/n이걸로 구분해서 body 파싱 진행
{
	Request req;

	std::istringstream iss(requestStr);
	std::string line;

	try {

		std::getline(iss, line);
		parseRequestLine(req, line);

		while (std::getline(iss, line) && !line.empty()) {
			parseHeader(req, line);
		}

		// if (method == "post")
		// 	body parse.

		// if (req.headers.find("Content-Length") != req.headers.end()) {
		// 	int contentLength = atoi(req.headers["Content-Length"].c_str());
		// 	std::string _body(contentLength, '\0');
		// 	iss.read(&_body[0], contentLength);
		// 	req._body = _body;
		// }

		std::cout << req._method << std::endl;
		std::cout << req._uri << std::endl;
		std::cout << req._version << std::endl;
		std::cout << "-------------" << std::endl;
		// std::cout << req._headers["Host"] << std::endl;

		isVaildRequest(req);
	}
	catch (std::invalid_argument& e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
		req._readStatus = READ_ERROR;
		return req;
	}
	return req;
}

void HttpRequest::initRequest(Request& req)
{
	req._method = "";
	req._uri = "";
	req._version = "";
	req._headers.clear();
	req._readStatus = READ_NOT_DONE;
	req._status = 0;
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


#endif

//telnet을 사용하는 경우도 고려해야함. 즉, 유효성 검사를 해줘야함.
//

// favicon을 만나면 not found.

// 400은 요쳥이 대강 이상한경우
// 403은 권한없는거 (실행못하는 mothod나 등등)
// mothod가 없는것일 경우 413 err.
// F_OK 가 아닐경우 404 존재 x.
// F_OK이지만 open,execute 등등 권한이 없을경우 403


//todo
/*
1. body parse (quarry로 들어옴 &로 구문하고 =로 key,value로 됨). (header다 읽고나서 line상황이 어떤지 확인 후 parseing 할 것)
1. body length 랑 header에 들어온 content-length랑 비교해서 덜 읽었다는 flag를 올려줌 (getter로 해줄지는 정할것,)
*/