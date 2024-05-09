
#pragma once



#include "../utils/utils.hpp"
#include <string>
#include <map>
#include <iostream>



class Response {
private:
	std::string _version;
	int _status;
	std::string _reason;
	std::string _body;
	std::string _location; // Redirect location
public:
    typedef std::multimap<std::string, std::string> Headers;
    Headers headers;

	Response() : _version("HTTP/1.1"), _status(-1) {
	// Information responses

	}

	void setVersion(const std::string& version) {
		_version = version;
	}

	std::string getVersion() const {
		return _version;
	}

	int getStatusCode() const {
		return _status;
	}

	std::string getReason() const {
		return _reason;

	}

    void setHeader(const std::string& name, const std::string& value) {
        headers.insert(std::make_pair(name, value));
    }

	void set_redirect(const std::string& url, int status = 302) {
		setHeader("Location", url);
		setStatusCode(status);
	}

	void setStatusCode(int status) {
		_reason = Error::errors[status];
		this->_status = status;
	}
		

    void remove_header(const std::string& name) {
        std::pair<Headers::iterator, Headers::iterator> range = headers.equal_range(name);
        headers.erase(range.first, range.second);
    }

    // 헤더 가져오기
    std::string get_header_value(const std::string& name) const {
        std::pair<Headers::const_iterator, Headers::const_iterator> range = headers.equal_range(name);
        if (range.first != headers.end()) {
            return range.first->second;
        }
        return std::string() = "";
    }

    // 모든 헤더 값 가져오기
    std::vector<std::string> get_header_values(const std::string& name) const {
        std::vector<std::string> values;
        std::pair<Headers::const_iterator, Headers::const_iterator> range = headers.equal_range(name);
        for (Headers::const_iterator it = range.first; it != range.second; ++it) {
            values.push_back(it->second);
        }
        return values;
    }

    // 헤더 존재 여부 확인
    bool has_header(const std::string& name) const {
        return headers.find(name) != headers.end();
    }

	void setBody(const std::string& body) {
		this->_body = body;
	}

	std::string get_responses() {
		std::string response = _version + " " + std::to_string(_status) + " " + Error::errors[_status] + "\r\n";
		for (std::multimap<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
			response += it->first + ": " + it->second + "\r\n";
		}
		response += "\r\n" + _body;
		return response;
	}
};

