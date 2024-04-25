#include "Config.hpp"
#include "ServerConfig.hpp"

// class LocationConfig {
//     private:
//     friend class Config;
//     friend class ServerConfig;
//     // std::unordered_map<std::string, std::string> fastcgi_params;
//     std::string                 _path;
//     std::string                 _alias;
//     std::string                 _root;
//     std::string                 _index;
//     bool                        _autoindex;
//     std::vector<std::string>    _allow_methods;
//     std::string                 _return_code;
//     std::string                 _return_url;
//     std::string                 _fastcgi_pass;
//     std::string                 _fastcgi_param;
//     std::string                 _upload_pass;
//     std::string                 _upload_store;
//     std::string                 _upload_state_store;
//     bool                        _upload_resume;
//     std::string                 _upload_max_file_size;
//     std::string                 _upload_limit_rate;
//     std::vector<int>            _upload_cleanup;
// 	std::vector<std::string>    _try_files;
    
    
//     public:
//     ~LocationConfig() {
//     }

//     // setter
//     void    setPath(const std::string &path)
// {
//     this->_path = path;
// }

// void    setAlias(const std::string &alias)
// {
//     this->_alias = alias;
// }

// void    setRoot(const std::string &root)
// {
//     this->_root = root;
// }

// void    setIndex(const std::string &index)
// {
//     this->_index = index;
// }

//     // void    setAutoindex(const std::string &autoindex);
//     // void    setAllowMethods(const std::string &allow_methods);
//     // void    setReturnCode(const std::string &return_code);
//     // void    setReturnUrl(const std::string &return_url);
//     // void    setFastcgiPass(const std::string &fastcgi_pass);\
//     // void    setFastcgiParam(const std::string &fastcgi_param);
//     // void    setUploadPass(const std::string &upload_pass);
//     // void    setUploadStore(const std::string &upload_store);
//     // void    setUploadStateStore(const std::string &upload_state_store);
//     // void    setUploadResume(const std::string &upload_resume);
//     // void    setUploadMaxFileSize(const std::string &upload_max_file_size);
//     // // void    setUploadLimitRate(const std::string &upload_limit_rate);
//     // void    setUploadCleanup(const std::vector<int> &upload_cleanup);
//     // void    setTryFiles(const std::vector<std::string> &try_files);

//     // getter
// std::string                 getPath() { return _path; }

// std::string                 getAlias() { return _alias; }

// std::string                 getRoot() {
//     if (_root.empty())
//         return ".";
//     return _root;
// }

// std::string                 getIndex() {
//     if (_index.empty())
//         return "index.html";
//     return _index;
// }
//     // bool                        getAutoindex() const;
//     // std::vector<std::string>    getAllowMethods() const;
//     // std::string                 getReturnCode() const;
//     // std::string                 getReturnUrl() const;
//     // std::string                 getFastcgiPass() const;
//     // std::string                 getUploadPass() const;
//     // std::string                 getUploadStore() const;
//     // std::string                 getUploadStateStore() const;
//     // bool                        getUploadResume() const;
//     // std::string                 getUploadMaxFileSize() const;
//     // std::string                 getUploadLimitRate() const;
//     static LocationConfig              parseLocation(std::ifstream &filename);
//     // std::vector<int>            getUploadCleanup() const;
//     // std::vector<std::string>    getTryFiles() const;
//     // // 추가적인 필요한 변수들
// };
// void replaceTabsWithSpaces(std::string& str);

// LocationConfig    LocationConfig::parseLocation(std::ifstream &file)
// {
//     std::string     line;
//     std::string     key, value;
//     LocationConfig  newLocation;
//     bool            inServerBlock = false;
//     bool            inLocationBlock = false;
//     while (std::getline(file, line)) {
//         if (line[0] == '}') {
//             file.seekg(-(line.length() + 1), std::ios::cur);
//             break;
//         }
//         replaceTabsWithSpaces(line);

//         std::istringstream iss(line);
//         std::string key;
        
//         if (!(iss >> key)) continue; // Skip empty lines

//         if (line == "server")   inServerBlock = true;
//         if (key == "location")  inLocationBlock = true;
        
//         if (line.find("}") != std::string::npos) {
//             if (inLocationBlock && inServerBlock) {
//                 inLocationBlock = false;
//             } else if (inLocationBlock == false) {
//                 inServerBlock = false;
//             }
//         }
//         if (key == "path") {
//             std::string path;
//             iss >> path;
//             if (path.empty())
//                 continue;
//             else
//                 newLocation._path = path;
//         } else if (key == "alias") {
//             std::string alias;
//             iss >> alias;
//             if (alias.empty())
//                 continue;
//             else
//                 newLocation._alias = alias;
//         } else if (key == "root") {
//             std::string root;
//             iss >> root;
//             if (root.empty())
//                 continue;
//             else
//                 newLocation._root = root;
//         } else if (key == "index") {
//             std::string index;
//             iss >> index;
//             if (index.empty())
//                 continue;
//             else
//                 newLocation._index = index;
//     //     } else if (key == "autoindex") {
//     //         std::string autoindex;
//     //         iss >> autoindex;
//     //         if (autoindex.empty())
//     //             continue;
//     //         // else
//     //         //     newLocation.autoindex = autoindex; // bool init
//     //     } else if (key == "allow_methods") {
//     //         std::string allow_methods;
//     //         iss >> allow_methods;
//     //         if (allow_methods.empty())
//     //             continue;
//     //         // else
//     //             // newLocation.setAllowMethods.push_back(allow_methods);
//     //     } else if (key == "return_code") {
//     //         std::string return_code;
//     //         iss >> return_code;
//     //         if (return_code.empty())
//     //             continue;
//     //         else
//     //             newLocation.setReturnCode(return_code);
//     //     } else if (key == "return_url") {
//     //         std::string return_url;
//     //         iss >> return_url;
//     //         if (return_url.empty())
//     //             continue;
//     //         else
//     //             newLocation.setReturnUrl(return_url);
//     //     } else if (key == "cgi_pass") {
//     //         std::string fastcgi_pass;
//     //         iss >> fastcgi_pass;
//     //         if (fastcgi_pass.empty())
//     //             continue;
//     //         else
//     //             newLocation.setFastcgiPass(fastcgi_pass);
//     //     } else if (key == "cgi_param") {
//     //         std::string fastcgi_param;
//     //         iss >> fastcgi_param;
//     //         if (fastcgi_param.empty())
//     //             continue;
//     //         else
//     //             newLocation.setFastcgiParam(fastcgi_param);
//     //     } else if (key == "upload_pass") {
//     //         std::string upload_pass;
//     //         iss >> upload_pass;
//     //         if (upload_pass.empty())
//     //             continue;
//     //         else
//     //             newLocation.setUploadPass(upload_pass);
//     //     } else if (key == "upload_store") {
//     //         std::string upload_store;
//     //         iss >> upload_store;
//     //         if (upload_store.empty())
//     //             continue;
//     //         else
//     //             newLocation.setUploadStore(upload_store);
//     //     } else if (key == "upload_state_store") {
//     //         std::string upload_state_store;
//     //         iss >> upload_state_store;
//     //         if (upload_state_store.empty())
//     //             continue;
//     //         else
//     //             newLocation.setUploadStateStore(upload_state_store);
//     //     } else if (key == "upload_resume") {
//     //         std::string upload_resume;
//     //         iss >> upload_resume;
//     //         if (upload_resume.empty())
//     //             continue;
//     //         // else
//     //         //     newLocation.upload_resume = upload_resume; // bool init
//     //     } else if (key == "upload_max_file_size") {
//     //         std::string upload_max_file_size;
//     //         iss >> upload_max_file_size;
//     //         if (upload_max_file_size.empty())
//     //             continue;
//     //         else
//     //             newLocation.setUploadMaxFileSize(upload_max_file_size);
//     //     } else if (key == "upload_limit_rate") {
//     //         std::string upload_limit_rate;
//     //         iss >> upload_limit_rate;
//     //         if (upload_limit_rate.empty())
//     //             continue;
//     //         else
//     //             newLocation.setUploadLimitRate(upload_limit_rate);
//     //     } else if (key == "upload_cleanup") {
//     //         std::string upload_cleanup;
//     //         iss >> upload_cleanup;
//     //         if (upload_cleanup.empty())
//     //             continue;
//     //         // else
//     //         //     newLocation.setUploadCleanup(upload_cleanup);
//     //     } else if (key == "try_files") {
//     //         std::string try_files;
//     //         iss >> try_files;
//     //         if (try_files.empty())
//     //             continue;
//     //         // else
//     //         //     newLocation.try_files.push_back(try_files);
//     //     }
//     // }
//         }
//     }
//     return newLocation;
// }

// std::unordered_map<int, std::string> parseErrorPages(std::istringstream &iss);

// class ServerConfig
// {
// public:
//     ~ServerConfig();
//     static ServerConfig parseServer(std::ifstream &file);
// std::string getServerName() {
//         return _server_name;
// }
// private :
//     friend class Config;
//     int _port;
//     std::string _server_name;
//     int _client_max_body_size;
//     std::unordered_map<int, std::string> _error_pages;
//     std::map<std::string, LocationConfig> _locations;
// };


// ServerConfig::~ServerConfig()
// {
// }


// void replaceTabsWithSpaces(std::string& str) {
//     std::replace(str.begin(), str.end(), '\t', ' ');  // replace all tabs with spaces
// }

// ServerConfig    ServerConfig::parseServer(std::ifstream &file)
// {
//     std::string     line;
//     ServerConfig    currentServer;
//     bool inServerBlock = false;

//     while (getline(file, line)) {
//         replaceTabsWithSpaces(line);
//         std::istringstream iss(line);
//         std::string key;
//         if (!(iss >> key)) continue; // Skip empty lines

//         if (line[0] ==  '}')
//         {
//             break ;
//         }
//         if (key == "server") {
//             inServerBlock = true;
//         } else if (key == "listen") {
//             std::string listen;
//             iss >> listen;
//             currentServer._port = std::stoi(listen);
//         } else if (key == "server_name") {
//             std::string server_name;
//             iss >> server_name;
//             currentServer._server_name = server_name;
//         } else if (key == "error_pages") {
//             currentServer._error_pages = parseErrorPages(iss); // 발생 가능할 에러 생각
//         } else if (key == "client_max_body_size") {
//             std::string client_max_body_size;
//             iss >> client_max_body_size;
//             currentServer._client_max_body_size = std::stoi(client_max_body_size);
//         } 
//         else if (key == "location") {
//             file.seekg(-(line.length() + 1), std::ios::cur);
//             LocationConfig location = LocationConfig::parseLocation(file);
//             currentServer._locations.insert(std::make_pair(location.getPath(), location));
//         }
//     }
//     return currentServer;
// }

// std::unordered_map<int, std::string> parseErrorPages(std::istringstream &iss) {
//     std::unordered_map<int, std::string> error_pages;
//     std::string error_code;
//     std::string error_page;
//     while (true) {
//         // if (!(iss >> error_code >> error_page)) return errno;
//         error_pages[std::stoi(error_code)] = error_page;
//         if (iss.eof()) break;
//     }
//     return error_pages;
// }


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
        if (line.find("server") != std::string::npos)
        {
            file.seekg(-(line.length() + 1), std::ios::cur);
            ServerConfig server(file);
            // std::cout << server.getServerName() << std::endl;
            _servers.push_back(server);
        }
    }
}

Config::Config(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Error: Invalid number of arguments" << std::endl;
        exit(1);
    }
    parseConfig(argv[1]);
}

Config::~Config()
{

}
using namespace std;

int main(int argc, char **argv)
{
    Config config(argc, argv);

    cout << "0000" << config._servers[0].getServerName() << endl;
    cout << "1111" << config._servers[1].getServerName() << endl;
    // cout << config._servers[0]._error_pages[404] << endl;
    return 0;
}