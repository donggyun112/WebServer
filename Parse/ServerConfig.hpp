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
    std::vector<LocationConfig> _locations;
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

    std::vector<LocationConfig> getLocation() {
        return _locations;
    }
    bool    isExtention(std::string httpPath) { 
        if (httpPath.find_last_of('.') == std::string::npos)
            return false;
        return true;
    }

    LocationConfig getLocation(std::string httpPath) {
                std::cout << "Http Path: " << httpPath << std::endl;
        if (isExtention(httpPath)) {
            httpPath = httpPath.substr(httpPath.find_last_of('.'), httpPath.size() - httpPath.find_last_of('.'));
        }
        
        for (size_t i = 0; i < _locations.size(); i++) {
            if (httpPath.substr(0, _locations[i].getPath().size()) == _locations[i].getPath())
            {
                if (httpPath.size() > _locations[i].getPath().size() && httpPath[_locations[i].getPath().size()] != '/')
                    continue;
                std::cout << "Location Path: " << _locations[i].getPath() << std::endl;
                return _locations[i];
            }
        }
		throw std::runtime_error("This path doesn't match with any location");
    }
};

#endif