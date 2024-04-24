#include "Config.hpp"

class LocationConfig;

class ServerConfig
{
private:
    int _port;
    std::string _server_name;
    std::map<std::string, LocationConfig> _locations;
public:
    ServerConfig(/* args */);
    ~ServerConfig();
    void    setPort(int port);
    void    setServerName(std::string server_name);
    void    setLocations(std::map<std::string, LocationConfig> locations);
};

ServerConfig::ServerConfig(/* args */)
{
}

ServerConfig::~ServerConfig()
{
}
