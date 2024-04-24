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
    std::vector<ServerConfig> _servers;
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

void    parseConfig(const std::string filename)
{
    std::string line;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    while (getline(file, line))
    {
        if (line.empty())
            continue;
        std::istringstream iss(line);
        if (line.find("server") != std::string::npos)
        {
            ServerConfig server = parseServer(filename);
        }
    }
}

ServerConfig parseServer(const std::string filename)
{
    std::string line;
    std::ifstream file(filename.c_str());
    ServerConfig server;
    bool inServerBlock = false;
    bool inLocationBlock = false;

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    while (getline(file, line))
    {
        std::string key;
        std::istringstream iss(line);

        if (line.empty())
            continue;
        if (!(iss >> key)) continue ;

        if (key == "server") {
            inServerBlock = true;
        }
        if (key == "listen") {
            std::string listen;
            iss >> listen;
            server.setPort(std::stoi(listen));
        } else if (key == "server_name") {
            std::string server_name;
            iss >> server_name;
            server.setServerName(server_name);
        } else if (key == "error_pages") {
            
        }

        // if (line.find("location") != std::string::npos)
        // {
        //     LocationConfig = parseServer(filename);
        // }
    }
}

Config::~Config()
{
}
