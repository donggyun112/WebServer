#include "../utils/utils.hpp"
// #include "../socket/socket.hpp"
#include <iostream>

class LocationConfig {
	public:
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
		const std::string getPath() const { return path; }
    // 추가적인 필요한 변수들
};

class ServerConfig {
		
	public:
		Port port;
		std::string server_name;
		std::unordered_map<int, std::string> error_pages;
		std::string client_max_body_size;
		std::map<std::string, LocationConfig> locations; // urls, LocationConfig
		ServerConfig() {}
		~ServerConfig() {}
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
		~Config() {}

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

int main() {
	Config conf;
	{
		try
		{
			// Port a = 1;
			// std:: cout << a + 1 << std::endl;
			// return 0;
			conf[1];
			conf["localhost"];
			conf[Port(8080)];
			
			conf.getServerConfig(Port(8080));
			conf.getServerConfig("localhost");
			conf.getServerConfig(1);

		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	{
		conf[1]["/"];
		conf["localhost"]["/"];
		conf[Port(8080)]["/"];

		conf.getServerConfig(Port(8080))["/"];
		conf.getServerConfig("localhost")["/"];
		conf.getServerConfig(1)["/"];

		conf.getServerConfig(Port(8080)).getLocationConfig("/");
		conf.getServerConfig("localhost").getLocationConfig("/");
		conf.getServerConfig(1).getLocationConfig("/");

		conf[1].getLocationConfig("/");
		conf["localhost"].getLocationConfig("/");
		conf[Port(8080)].getLocationConfig("/");
	}
		
	
}