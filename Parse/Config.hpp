#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iostream>

class ServerConfig;
class LocationConfig;

class Config
{
private:
    std::map<std::string, ServerConfig> _servers;
public:
    Config(/* args */);
    Config(int argc, char **argv);
    ~Config();
    void setServers(const std::map<std::string, ServerConfig> servers);
    std::map<std::string, ServerConfig> getServers();
};

Config::Config(/* args */)
{
}

void    parseConfig(const std::string &filename)
{
    // parseServer(filename);
}

Config::~Config()
{
}
