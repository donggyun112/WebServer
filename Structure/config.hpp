#include "../utils/utils.hpp"
#include "../socket/socket.hpp"
#include <iostream>

struct LocationConfig {
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
    // 추가적인 필요한 변수들
};

struct ServerConfig {
    int port;
    std::string server_name;
    std::unordered_map<int, std::string> error_pages;
    std::string client_max_body_size;
    std::map<std::string, LocationConfig> locations; // urls, LocationConfig
    // ...
};

class IConfig {
	public:
		virtual ~IConfig() {}
		virtual void registerSocket(const FD &socket) = 0;
		virtual void dispose(const FD &socket) = 0;
};