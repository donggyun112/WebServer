#ifndef CONFIG_HPP
# define CONFIG_HPP

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

class Config
{
private:
public:
    std::vector<ServerConfig> _servers;
    Config(int argc, char **argv);

    ~Config();
    void setServers(const std::map<std::string, ServerConfig> servers);
    std::map<std::string, ServerConfig> getServers();

    void    parseConfig(const std::string filename);
};

#endif