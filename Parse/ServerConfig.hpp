#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include "LocationConfig.hpp"
#include "../utils/utils.hpp"
#include <regex>

class ServerConfig
{
private:
    Port _port;
    std::string _server_name;
    size_t _client_max_body_size;
    std::vector<LocationConfig> _locations;
    ServerConfig();
    std::string _path;
    std::string _error_pages_path;
    std::string _index;
public:
    ServerConfig(std::ifstream &file);
    ~ServerConfig();
    std::unordered_map<int, std::string> parseErrorPages(std::istringstream &iss);
    std::string getServerName() const;
    std::string getPath() const { 
		return _path;
	}
	 
    Port getPortName() const;
    size_t getNumberOfLocation();
	size_t getClientMaxBodySize() const ;
    
    LocationConfig getLocation(int i) {
        return _locations[i];
    }

    bool    isExtention(std::string httpPath) {
        if (httpPath.find_last_of('.') == std::string::npos)
            return false;
        return true;
    }


	void	setPathInfo(std::string &httpPath) {
		
		if (httpPath.find('?') != std::string::npos) {
			setenv("QUERY_STRING", httpPath.substr(httpPath.find('?') + 1).c_str(), 1);
			httpPath = httpPath.substr(0, httpPath.find('?'));
		}
		else if (httpPath.find('/') != std::string::npos) {
			setenv("PATH_INFO", httpPath.substr(httpPath.find('/')).c_str(), 1);
			httpPath = httpPath.substr(0, httpPath.find('/'));
		}
	}

	bool isConfPattern(const std::string& str) {
		std::stringstream ss(str);
		std::string token;
		
		while (std::getline(ss, token, '$')) {}
		if (token != "") {
			return false;
		}
		
		ss.clear();
		ss.seekg(0);
		
		if (!std::getline(ss, token, '\\') || token != "") {
			return false;
		}
		
		if (!std::getline(ss, token, '$') || token[0] != '.') {
			return false;
		}
		
		for (size_t i = 1; i < token.length(); i++) {
			if (!isalnum(token[i]) && token[i] != '.') {
				return false;
			}
		}
		
		return true;
	}

	std::vector<std::string> getHttpUriToken(std::string httpPath) {
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream iss(httpPath);
		while (std::getline(iss, token, '/')) {
			tokens.push_back("/" + token);
		}
		return tokens;
	}

    LocationConfig getLocation(std::string httpPath) {
        std::string extention;
		LocationConfig notFound;
		bool isLocation = false;
		
        
		std::vector<std::string> tokens = getHttpUriToken(httpPath);
		for (size_t j = 0; j < tokens.size(); j++) {
			for (size_t i = 0; i < _locations.size(); i++) {
				if (tokens[j] == _locations[i].getPath())
				{
					if (tokens[j] == "/") {
						notFound = _locations[i];
						isLocation = true;
						continue;
					}
					std::cout << "Location " << _locations[i].getPath() << std::endl;
					return _locations[i];
				}
				else if (isExtention(tokens[j]))
				{
					extention = httpPath.substr(httpPath.find_last_of('.'), httpPath.size() - httpPath.find_last_of('.'));
					extention = extention.substr(0, extention.find_first_of('/'));
					extention = "\\" + extention + "$";
					if (isConfPattern(extention)) {

						for (size_t i = 0; i < _locations.size(); ++i) {
							if (_locations[i].getPath() == extention)
							{
								return _locations[i];
							}
						}
					}
				}
			}
		}
		if (isLocation)
			return notFound;
		throw std::runtime_error("This path doesn't match with any location");
    }
    std::string getErrorPagesPath() const {
        return _error_pages_path;
    }

	ServerConfig &operator=(const ServerConfig &serverConfig) {
		if (this != &serverConfig) {
			_port = serverConfig._port;
			_server_name = serverConfig._server_name;
			_client_max_body_size = serverConfig._client_max_body_size;
			_locations = serverConfig._locations;
			_path = serverConfig._path;
			_error_pages_path = serverConfig._error_pages_path;
			_index = serverConfig._index;
		}
		return *this;
	}

};

#endif