#include "ServerConfig.hpp"

ServerConfig::~ServerConfig()
{
}

void replaceTabsWithSpaces(std::string& str) {
    std::replace(str.begin(), str.end(), '\t', ' ');  // replace all tabs with spaces
}

ServerConfig    ServerConfig::parseServer(std::ifstream &file)
{
    std::string     line;
    ServerConfig    currentServer;
    bool inServerBlock = false;

    while (getline(file, line)) {
        replaceTabsWithSpaces(line);
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)) continue; // Skip empty lines

        if (line[0] ==  '}')
        {
            break ;
        }
        if (key == "server") {
            inServerBlock = true;
        } else if (key == "listen") {
            std::string listen;
            iss >> listen;
            currentServer._port = std::stoi(listen);
        } else if (key == "server_name") {
            std::string server_name;
            iss >> server_name;
            currentServer._server_name = server_name;
        } else if (key == "error_pages") {
            currentServer._error_pages = parseErrorPages(iss); // 발생 가능할 에러 생각
        } else if (key == "client_max_body_size") {
            std::string client_max_body_size;
            iss >> client_max_body_size;
            currentServer._client_max_body_size = std::stoi(client_max_body_size);
        } 
        else if (key == "location") {
            file.seekg(-(line.length() + 1), std::ios::cur);
            LocationConfig location = LocationConfig::parseLocation(file);
            currentServer._locations.insert(std::make_pair(location.getPath(), location));
        }
    }
    return currentServer;
}

std::unordered_map<int, std::string> ServerConfig::parseErrorPages(std::istringstream &iss) {
    std::unordered_map<int, std::string> error_pages;
    std::string error_code;
    std::string error_page;
    while (true) {
        // if (!(iss >> error_code >> error_page)) return errno;
        error_pages[std::stoi(error_code)] = error_page;
        if (iss.eof()) break;
    }
    return error_pages;
}
