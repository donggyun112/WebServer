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

class ServerConfig
{
private :
    int _port;
    std::string _server_name;
    int _client_max_body_size;
    std::unordered_map<int, std::string> _error_pages;
    std::map<std::string, LocationConfig> _locations;
    ServerConfig();
public:
    ServerConfig(std::ifstream &file);
    ~ServerConfig();
    std::unordered_map<int, std::string> parseErrorPages(std::istringstream &iss);
    std::string getServerName() const;
    int getPortName() const;
};

#endif