#include "request.hpp"

void HttpRequest::parse(const std::string& request)
{
	std::istringstream iss(request);
	std::string line;

	// 1. 요청 라인 파싱
	std::getline(iss, line);
	parseRequestLine(line);

	// 2. 요청 헤더 파싱
	while (std::getline(iss, line) && !line.empty()) {
		parseHeader(line);
	}

	// 3. 요청 본문 파싱 (선택적)
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
