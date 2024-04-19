#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <Config.hpp>

struct ServerConfig Server;

// void weWantJustOneSpace(std::string& str) {
//     std::string::iterator new_end = std::unique(str.begin(), str.end(), bothAreSpaces);
//     str.erase(new_end, str.end());
// }

void replaceTabsWithSpaces(std::string& str) {
    std::replace(str.begin(), str.end(), '\t', ' ');  // replace all tabs with spaces
}

void    parseLocation(const std::string &filename)
{
    std::ifstream   file(filename.c_str());
    std::string     line;
    std::string     key, value;

}

// server 블록 안에 로케이션 들어있는게 하나 의 짝이다
// parse 에서 server 블록을 push_back 해줘야하기에 
// 만들어서 슥 ..   

void    setPort(const std::string &port)
{
    Server.port = std::stoi(port);
}

void    setServerName(const std::string &server_name)
{
    Server.server_name = server_name;
}
// location block 에다가 넣어줘야함.
// void    setAutoindex(const std::string &autoindex)
// {
//     Server.autoindex = autoindex;
// }

// void    setIndex(const std::string &index)
// {
//     Server.index = index;
// }

void    setClientBodyBufferSize(const std::string &client_body_buffer_size)
{
    Server.client_body_buffer_size = client_body_buffer_size;
}

void    set

void    parseServer(const std::string &filename)
{
    std::ifstream                           file(filename.c_str());
    std::string                             line;
    std::string                             key, value;
    ServerConfig                            newServer;
    bool                                    inServerBlock = false;

    while (getline(file, line)) {
        replaceTabsWithSpaces(line);
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)) continue; // Skip empty lines

        if (key == "location") {
            parseLocation(filename);
        }

        if (key == "server") {
            if (inServerBlock) {
                Server(newServer); // 생성자 만들어줘야함 내부는 
                inServerBlock = false;
            }
            inServerBlock = true;
        }
        if (key == "listen") {
            std::string port;
            iss >> port;
            if (!port)
                continue;
            else 
                newServer.setPort(std::stoi(port));
        } else if (key == "server_name") {
            std::string server_name;
            iss >> server_name;
            if (!server_name)
                continue;
            else
                newServer.setServerName(server_name);
        } else if (key == "autoindex") {
            std::string autoindex;
            iss >> autoindex;
            if (!autoindex)
                continue;
            else
                newServer.setAutoindex(autoindex);
        } else if (key == "index") {
            std::string index;
            iss >> index;
            if (!index)
                continue;
            else
                newServer.setIndex(index);
        } else if (key == "client_body_buffer_size") {
            std::string client_body_buffer_size;
            iss >> client_body_buffer_size;
            if (!client_body_buffer_size)
                continue;
            else
                newServer.setClientBodyBufferSize(client_body_buffer_size);
        } else if (key == "cgi_param") {
            std::string cgi_param;
            iss >> cgi_param;
            if (!cgi_param)
                continue;
            else
                newServer.setCgiParam(cgi_param);
        }
    }
}

void    parseConfig(const std::string &filename)
{
    parseServer(filename);
    parseLocation(filename);
}
int main()
{
    parseConfig("nginx.conf");
    
    return 0;
}