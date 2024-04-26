#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "Config.hpp"
#include "LocationConfig.hpp"

class ServerConfig
{
private :
    friend class Config;
    int _port;
    std::string _server_name;
    int _client_max_body_size;
    std::unordered_map<int, std::string> _error_pages;
    std::map<std::string, LocationConfig> _locations;
public:
    ~ServerConfig();
    std::unordered_map<int, std::string> parseErrorPages(std::istringstream &iss);
    std::string getServerName();
    ServerConfig(std::ifstream &file);
};

#endif