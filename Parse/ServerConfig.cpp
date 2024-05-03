#include "ServerConfig.hpp"
#include "Config.hpp"

ServerConfig::~ServerConfig() {}

std::string ServerConfig::getServerName() const{return this->_server_name;}

Port ServerConfig::getPortName() const {return _port;}

ServerConfig::ServerConfig(std::ifstream &file)
{
    std::string     line;
    // bool inServerBlock = false;

    while (getline(file, line)) {
        replaceTabsWithSpaces(line);
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)) continue; // Skip empty lines

        if (line[0] ==  '}') break ;
        
        // if (key == "server") {
        //     inServerBlock = true;}
        if (key == "path") {
            iss >> this->_path;
        }
        else if (key == "listen") {
            std::string listen;
            iss >> listen;
            this->_port = static_cast<Port>(std::atoi(listen.c_str()));
        } else if (key == "server_name") {
            std::string server_name;
            iss >> server_name;
            this->_server_name = server_name;
        } else if (key == "error_pages") {
            this->_error_pages = parseErrorPages(iss); // 발생 가능할 에러 생각
        } else if (key == "client_max_body_size") {
            std::string client_max_body_size;
            iss >> client_max_body_size;
            this->_client_max_body_size = std::atoi(client_max_body_size.c_str());
        } 
        else if (key == "location") {
            file.seekg(-(line.length() + 1), std::ios::cur);
            LocationConfig location(file);
            this->_locations.insert(std::make_pair(location.getPath(), location));
        }
    }
}

std::unordered_map<int, std::string> ServerConfig::parseErrorPages(std::istringstream &iss) {
    std::unordered_map<int, std::string> error_pages;
    std::string error_code;
    std::string error_page;
    while (true) {
        // if (!(iss >> error_code >> error_page)) return errno;
        error_pages[std::atoi(error_code.c_str())] = error_page;
        if (iss.eof()) break;
    }
    return error_pages;
}
