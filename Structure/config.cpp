#include "../utils/utils.hpp"
// #include "../socket/socket.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class LocationConfig {
	private:
		std::string path;
		std::string alias;
		std::string root;
		std::string index;
		bool autoindex;
		std::vector<std::string> allow_methods;
		std::string return_code;
		std::string return_url;

		std::string fastcgi_pass;
		std::unordered_map<std::string, std::string> fastcgi_params;

		std::string upload_pass;
		std::string upload_store;
		std::string upload_state_store;
		bool upload_resume;
		std::string upload_max_file_size;
		std::string upload_limit_rate;
		std::vector<int> upload_cleanup;

		std::vector<std::string> try_files;
		friend class Config;
	public:
		const std::string getPath() const { return path; }
    // 추가적인 필요한 변수들
};

class ServerConfig {
		
	private:
		Port port;
		std::string server_name;
		std::unordered_map<int, std::string> error_pages;
		std::string client_max_body_size;
		std::map<std::string, LocationConfig> locations; // urls, LocationConfig
		friend class Config;
	public:
		ServerConfig() {}
		~ServerConfig() {
			locations.clear();
		}
		std::string getServerName() const { return server_name; }


		const LocationConfig operator[](const char *url) const {
			std::map<std::string, LocationConfig>::const_iterator it = locations.find(url);
			if (it == locations.end()) {
				throw std::runtime_error("url not found");
			}
			return it->second;
		}

		const LocationConfig operator[](const std::string &url) const {
			std::map<std::string, LocationConfig>::const_iterator it = locations.find(url);
			if (it == locations.end()) {
				throw std::runtime_error("url not found");
			}
			return it->second;
		}


		const LocationConfig getLocationConfig(const std::string &url) const {
			std::map<std::string, LocationConfig>::const_iterator it = locations.find(url);
			if (it == locations.end()) {
				throw std::runtime_error("url not found");
			}
			return it->second;
		}

		const LocationConfig getLocationConfig(const char *url) const {
			std::map<std::string, LocationConfig>::const_iterator it = locations.find(url);
			if (it == locations.end()) {
				throw std::runtime_error("url not found");
			}
			return it->second;
		}

    
};

class Config {
	public:
		

		std::vector<ServerConfig> servers;
		Config(const char *filename) {
			configParse(filename);
		}
		~Config() {
			servers.clear();
		}


		void configParse(const char *filename) {
			std::ifstream file;	
			std::stringstream ss;
			file.open(filename, std::ios::in);

			while (!file.eof()) {
				std::string line;
				std::getline(file, line);
				if (line.find("server") != std::string::npos) {
					ServerConfig server = extractServerConfig(file);
					addServerConfig(server);
				}
			}
			
		}

		ServerConfig getServerDefalut() {
			ServerConfig server;

			server.port = 8080;
			server.server_name = "localhost";
			server.error_pages[404] = "404.html";
			server.error_pages[500] = "500.html";
			server.client_max_body_size = "1M";

			memset(&server.locations, 0, sizeof(server.locations));
			return server;
		}


		void addServerConfig(const ServerConfig &server) {
			servers.push_back(server);
		}

		ServerConfig extractServerConfig(std::ifstream &file) {
			ServerConfig server;

			while (!file.eof()) {
				std::string line;
				std::getline(file, line);
				if (line == "}") {
					break;
				}
				if (line == "port") {
					std::getline(file, line);
					server.port = std::stoi(line);
				} else if (line == "server_name") {
					std::getline(file, line);
					server.server_name = line;
				} else if (line == "error_pages") {
					while (true) {
						std::getline(file, line);
						if (line == "}") {
							break;
						}
						std::string key = line.substr(0, line.find(" "));
						std::string value = line.substr(line.find(" ") + 1);
						server.error_pages[std::stoi(key)] = value;
					}
				} else if (line == "client_max_body_size") {
					std::getline(file, line);
					server.client_max_body_size = line;
				} else if (line == "location") {
					LocationConfig location = extractLocationConfig(file);
					server.locations.insert(std::make_pair(location.path, location));

				}
			}
			return server;
		}

		// void addLocationConfigAuto(const LocationConfig &location, ServerConfig &server) {
		// 	try
		// 	{
		// 		addLocationConfig(server.server_name, location);
		// 	}
		// 	catch (const std::exception& e)
		// 	{
		// 		try
		// 		{
		// 			addLocationConfig(server.port, location);
		// 		}
		// 		catch (const std::exception& e)
		// 		{
		// 			try
		// 			{
		// 				addLocationConfig(static_cast<int>(servers.size() - 1), location);
		// 			}
		// 			catch(const std::exception& e)
		// 			{
		// 				try
		// 				{
		// 				}
		// 				catch(const std::exception& e)
		// 				{
		// 					std::cerr << e.what() << '\n';
		// 				}
		// 			}
					
		// 		}
				
		// 	}
		// }

		// void addLocationConfig(int idx, const LocationConfig &Location) {
		// 	servers[idx].locations.insert(std::make_pair(Location.path, Location));
		// }

		// void addLocationConfig(const std::string &server_name, const LocationConfig &Location) {
		// 	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); ++it) {
		// 		if (it->server_name == server_name) {
		// 			it->locations.insert(std::make_pair(Location.path, Location));
		// 			return;
		// 		}
		// 	}
		// 	throw std::runtime_error("server_name not found");
		// }

		// void addLocationConfig(const Port &port, const LocationConfig &Location) {
		// 	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); ++it) {
		// 		if (it->port == port) {
		// 			it->locations.insert(std::make_pair(Location.path, Location));
		// 			return;
		// 		}
		// 	}
		// 	throw std::runtime_error("port not found");
		// }

		const LocationConfig extractLocationConfig(std::ifstream &file) {
			LocationConfig location;
			
			while (!file.eof()) {
				std::string line;
				std::getline(file, line);
				if (line == "}") {
					break;
				}
				if (line == "path") {
					std::getline(file, line);
					location.path = line;
				} else if (line == "alias") {
					std::getline(file, line);
					location.alias = line;
				} else if (line == "root") {
					std::getline(file, line);
					location.root = line;
				} else if (line == "index") {
					location.index = line;
				} else if (line == "autoindex") {
					location.autoindex = true;
				} else if (line == "allow_methods") {
					location.allow_methods.clear();
					while (true) {
						std::getline(file, line);
						if (line == "}") {
							break;
						}
						location.allow_methods.push_back(line);
					}
				} else if (line == "return") {
					location.return_code = line;
					std::getline(file, line);
					location.return_url = line;
				} else if (line == "fastcgi_pass") {
					location.fastcgi_pass = line;
					while (true) {
						std::getline(file, line);
						if (line == "}") {
							break;
						}
						std::string key = line.substr(0, line.find(" "));
						std::string value = line.substr(line.find(" ") + 1);
						location.fastcgi_params.insert(std::make_pair(key, value));
					}
				} else if (line == "upload") {
					location.upload_pass = line;
				}
			}
			return location;
		}

		const ServerConfig& operator[](int index) const {
			std::cout << "index: " << index << "\n";
			if (index < 0 || index >= servers.size()) {
				throw std::runtime_error("index out of range");
			}

			return servers[index];
		}

		const ServerConfig& operator[](const char *server_name) const {
			std::cout << "server_name: " << server_name << "\n";
			if (servers.size() == 0) {
				throw std::runtime_error("server_name not found");
			}
			for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
				if (it->server_name.c_str() == server_name) {
					return *it;
				}
			}
			throw std::runtime_error("server_name not found");
		}

		const ServerConfig& operator[](const Port port) const {
			std::cout << "port: " << port << "\n";
			if (servers.size() == 0) {
				throw std::runtime_error("server_name not found");
			}
			for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
				if (it->port == port) {
					return *it;
				}
			}
			throw std::runtime_error("server_name not found");
		}

		ServerConfig getServerConfig(const std::string &server_name) const {
			
			if (servers.size() == 0) {
				throw std::runtime_error("server_name not found");
			}
			for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
				if (it->server_name == server_name) {
					return *it;
				}
			}
			throw std::runtime_error("server_name not found");
		}

		ServerConfig getServerConfig(const Port port) const {
			if (servers.size() == 0) {
				throw std::runtime_error("server_name not found");
			}
			for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
				if (it->port == port) {
					return *it;
				}
			}
			throw std::runtime_error("server_name not found");
		}

		ServerConfig getServerConfig(int index) const {
			if (index < 0 || index >= servers.size()) {
				throw std::runtime_error("index out of range");
			}
			return servers[index];
		}

		int getServerCount() const {
			return servers.size();
		}
};

int main(int ac, char **av) {
	Config conf(av[1]);
	std::cout << conf[0].getServerName() << std::endl;
	// {
	// 	try
	// 	{
	// 		// Port a = 1;
	// 		// std:: cout << a + 1 << std::endl;
	// 		// return 0;
	// 		conf[1];
	// 		conf["localhost"];
	// 		conf[Port(8080)];
			
	// 		conf.getServerConfig(Port(8080));
	// 		conf.getServerConfig("localhost");
	// 		conf.getServerConfig(1);

	// 	}
	// 	catch(const std::exception& e)
	// 	{
	// 		std::cerr << e.what() << '\n';
	// 	}
	// }
	// {
	// 	conf[1]["/"];
	// 	conf["localhost"]["/"];
	// 	conf[Port(8080)]["/"];

	// 	conf.getServerConfig(Port(8080))["/"];
	// 	conf.getServerConfig("localhost")["/"];
	// 	conf.getServerConfig(1)["/"];

	// 	conf.getServerConfig(Port(8080)).getLocationConfig("/");
	// 	conf.getServerConfig("localhost").getLocationConfig("/");
	// 	conf.getServerConfig(1).getLocationConfig("/");

	// 	conf[1].getLocationConfig("/");
	// 	conf["localhost"].getLocationConfig("/");
	// 	conf[Port(8080)].getLocationConfig("/");
	// }	
}