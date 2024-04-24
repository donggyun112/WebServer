#include "ServerConfig.hpp"

class ServerConfig
{
public:
    int _port;
    std::string _server_name;
    int _client_max_body_size;
    std::unordered_map<int, std::string> _error_pages;
    std::map<std::string, LocationConfig> _locations;
    ServerConfig(/* args */);
    ~ServerConfig();
    friend Config;
};

ServerConfig::ServerConfig(/* args */)
{
}

ServerConfig::~ServerConfig()
{
}


void replaceTabsWithSpaces(std::string& str) {
    std::replace(str.begin(), str.end(), '\t', ' ');  // replace all tabs with spaces
}

ServerConfig    parseServer(const std::string &filename)
{
    std::ifstream   file(filename.c_str());
    std::string     line;
    ServerConfig    currentConfig;
    bool inServerBlock = false;

    while (getline(file, line)) {
        replaceTabsWithSpaces(line);
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)) continue; // Skip empty lines

        if (key == "server") {
            if (inServerBlock) {
                currentConfig = ServerConfig(); // Reset
            }
            inServerBlock = true;
        } else if (key == "listen") {
            std::string listen;
            iss >> listen;
            currentConfig._port = std::stoi(listen);
        } else if (key == "server_name") {
            std::string server_name;
            iss >> server_name;
            currentConfig._server_name = server_name;
        } else if (key == "error_pages") {
            currentConfig._error_pages = parseErrorPages(iss); // 발생 가능할 에러 생각
        } else if (key == "client_max_body_size") {
            std::string client_max_body_size;
            iss >> client_max_body_size;
            currentConfig._client_max_body_size = std::stoi(client_max_body_size);
        } 
        // else if (key == "location") {
        //     LocationConfig location = parseLocation(iss);
        //     currentConfig._locations[location._path] = location; }
        else if (key == "}") {
            inServerBlock = false;
        }
        if (key == "}") {
            inServerBlock = false;
        }
    }
    return currentConfig;
}

std::unordered_map<int, std::string> parseErrorPages(std::istringstream &iss) {
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