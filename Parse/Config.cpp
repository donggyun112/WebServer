#include "Config.hpp"

void replaceTabsWithSpaces(std::string& str) {
    std::replace(str.begin(), str.end(), '\t', ' ');  // replace all tabs with spaces
}

void    Config::parseConfig(const std::string filename)
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
        if (line.find("server") != std::string::npos && line[0] != '#')
        {
            file.seekg(-(line.length() + 1), std::ios::cur);
            ServerConfig server(file);
            // std::cout << "sec" << server.getServerName() << std::endl;
            _servers.push_back(server);
        }
    }
}

Config::Config(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Error: Invalid number of arguments" << std::endl;
        parseConfig("../parse/nginx.conf");
    }
    parseConfig(argv[1]);
}

Config::~Config() {}

const ServerConfig& Config::operator[](int index) const {
	if (index < 0 || index >= _servers.size()) {
		throw std::runtime_error("Server Config | index out of range");
	}
	return _servers[index];
}

int Config::getNumberOfServer() const {
    return _servers.size();
}