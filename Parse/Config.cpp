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
        std::string key;
        std::string value;
        iss >> key;
        iss >> value;
        if (key == "server"  && value == "{" && line[0] != '#')
        {
            file.seekg(-(line.length() + 1), std::ios::cur);
            ServerConfig server(file);
            // std::cout << "sec" << server.getServerName() << std::endl;
            if (server.isValid())
                _servers.push_back(server);
        }
    }
    file.close();
}

void	Config::setServerName(std::string serverName)
{
	_serverNmae = serverName;
}

Config::Config(int argc, char **argv) : _servers() {
    if (argc != 2) {
        std::cerr << "Error: Invalid number of arguments" << std::endl;
        parseConfig("nginx.conf");
    }
    parseConfig(argv[1]);
}

Config::~Config() {}

ServerConfig Config::operator[](Port port) const {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i].getPortName() == port && _servers[i].getServerName().empty())
            return _servers[i];
    }

	for (size_t i = 0; i < _servers.size(); ++i) {
		if (_servers[i].getPortName() == port)
			return _servers[i];
	}
    throw std::runtime_error("This port doesn't match with any server");
	// return _servers[0];
}

ServerConfig Config::getServerConfig(Port port, const std::string &serverName) const {
	for (size_t i = 0; i < _servers.size(); ++i) {
		if (_servers[i].getPortName() == port && _servers[i].getServerName() == serverName)
			return _servers[i];
	}
	return this->operator[](port);
}

ServerConfig Config::operator[](int i) const {
    return _servers[i];
}

int Config::getNumberOfServer() const {
    return _servers.size();
}