#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include "LocationConfig.hpp"
#include "../utils/utils.hpp"

class ServerConfig
{
private:
    Port _port;
    std::string _server_name;
    int _client_max_body_size;
    std::unordered_map<int, std::string> _error_pages;
    std::map<std::string, LocationConfig> _locations;
    ServerConfig();
    std::string _path;
    std::string _index;
public:
    ServerConfig(std::ifstream &file);
    ~ServerConfig();
    std::unordered_map<int, std::string> parseErrorPages(std::istringstream &iss);
    std::string getServerName() const;
    std::string getPath() const { return _path; }
    Port getPortName() const;

    //getLocation 필요해서 만드는중인데스
    LocationConfig getLocation(std::string httpPath) {
        std::map<std::string, LocationConfig>::iterator it = _locations.begin();

        for (; it != _locations.end(); it++) {
            if (httpPath.substr(0, it->first.length()) == it->first)
                return it->second;
        }
		throw std::runtime_error("This path doesn't match with any location");
    }

	LocationConfig operator[](std::string path) const {
		if (_locations.find(path) != _locations.end()) {
			return _locations.at(path);
		}
		throw std::runtime_error("This path doesn't match with any location");
	}
};

#endif