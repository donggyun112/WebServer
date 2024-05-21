#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include <string>
# include <map>
# include <fstream>
# include <sstream>
# include <iostream>
# include <algorithm>
# include "ServerConfig.hpp"
# include "../utils/utils.hpp"


class Config
{
private:
    std::vector<class ServerConfig> _servers;
	std::string _serverNmae;

public:
    Config(int argc, char **argv);

    ~Config();
    // void setServers(const std::map<std::string, ServerConfig> servers);
	void 		setServerName(std::string serverName);
    ServerConfig operator[](Port port) const;
    ServerConfig operator[](int i) const;
	ServerConfig getServerConfig(Port port, const std::string &serverName) const;
    
    bool    preConfigCheck(const std::string filename);
    void    parseConfig(const std::string filename);
    int getNumberOfServer() const;
};
bool validateLocationConfig(const std::string &line);
void replaceTabsWithSpaces(std::string& str);
#endif
