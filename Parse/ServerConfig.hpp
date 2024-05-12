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
    size_t getNumberOfLocation();


    
    //용도에 맞게 바꿨으니 이거 지우지 마시오.
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
			std::cout << "PATH_INFO: " << getenv("PATH_INFO") << std::endl;
		}
	}
	// LocationConfig에서 이걸 처리하는게 맞는지 모르겠음 보통이런건 request에서 처리하는게 맞는거 같은데


    LocationConfig getLocation(std::string httpPath) {
        std::cout << "Http Path1: " << httpPath << std::endl;
        if (isExtention(httpPath)) {
			httpPath = httpPath.substr(httpPath.find_last_of('.'), httpPath.size() - httpPath.find_last_of('.'));
        }
        //  else {
            // httpPath = this->_locations[0].getRoot() + httpPath;
        // std::cout << "Http Path2: " << httpPath << std::endl;
// 
        // }      
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