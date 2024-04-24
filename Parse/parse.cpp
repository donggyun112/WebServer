#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include "Config.hpp"

struct ServerConfig     Server;
struct LocationConfig   Location;

// server 블록 안에 로케이션 들어있는게 하나 의 짝이다
// parse 에서 server 블록을 push_back 해줘야하기에 
// 만들어서 슥 ..   

void    parseServer(const std::string &filename)
{
    std::ifstream       file(filename.c_str());
    std::string         line;
    std::string         key, value;
    // IConfigParser       parser;
    ServerConfig        newServer;
    bool                inServerBlock = false;

    while (getline(file, line)) {
        replaceTabsWithSpaces(line);
        std::istringstream iss(line);
        std::string key;

        if (iss.eof())
            return ;
        if (!(iss >> key)) continue; // Skip empty lines

        // if (key == "location") {
        //     parseLocation(filename);
        // })

        if (key == "server") {
            if (inServerBlock) {
                // parser.ㄴetServerConfig(newServer);
                 // 생성자 만들어줘야함 내부는
            }
        }
        if (key == "listen") {
            std::string port;
            iss >> port;
            if (port.empty())
                continue;
            else
                newServer.port = std::stoi(port);
        } else if (key == "server_name") {
            std::string server_name;
            iss >> server_name;
            if (server_name.empty())
                continue;
            else
                newServer.server_name = server_name;
        } else if (key == "client_max_body_size") {
            std::string client_max_body_size;
            iss >> client_max_body_size;
            if (client_max_body_size.empty())
                continue;
            else
                newServer.client_max_body_size = client_max_body_size;
        }
        // } else if (key == "autoindex") {
        //     std::string autoindex;
        //     iss >> autoindex;
        //     if (!autoindex)
        //         continue;
        //     else
        //         newServer.autoindex = autoindex;
        // } else if (key == "index") {
        //     std::string index;
        //     iss >> index;
        //     if (!index)
        //         continue;
        //     else
        //         newServer.index = index;
        // } else if (key == "client_body_buffer_size") {
        //     std::string client_body_buffer_size;
        //     iss >> client_body_buffer_size;
        //     if (!client_body_buffer_size)
        //         continue;
        //     else
        //         newServer.client_body_buffer_size = client_body_buffer_size;
        // } else if (key == "cgi_param") {
        //     std::string cgi_param;
        //     iss >> cgi_param;
        //     if (!cgi_param)
        //         continue;
        //     else
        //         newServer.cgi_param = cgi_param;
        // }
    }
    Server = newServer;
    return ;
}
using namespace std;

int main()
{
    parseConfig("nginx.conf");
    cout << "Port >> " << Server.port << endl;
    cout << "Server Name >> " << Server.server_name << endl;
    cout << "Client Max Body Size >> " << Server.client_max_body_size << endl;

    cout << "Path >> " << Location.path << endl;
    cout << "Alias >> " << Location.alias << endl;
    cout << "Root >> " << Location.root << endl;
    cout << "Index >> " << Location.index << endl;
    cout << "Autoindex >> " << Location.autoindex << endl;
    cout << "Allow Methods >> " << Location.allow_methods[0] << endl;


    cout << "Return Code >> " << Location.return_code << endl;
    cout << "Return URL >> " << Location.return_url << endl;
    cout << "Fastcgi Pass >> " << Location.fastcgi_pass << endl;
    cout << "Fastcgi Param >> " << Location.fastcgi_param << endl;
    cout << "Upload Pass >> " << Location.upload_pass << endl;
    cout << "Upload Store >> " << Location.upload_store << endl;
    cout << "Upload State Store >> " << Location.upload_state_store << endl;
    cout << "Upload Resume >> " << Location.upload_resume << endl;
    cout << "Upload Max File Size >> " << Location.upload_max_file_size << endl;
    cout << "Upload Limit Rate >> " << Location.upload_limit_rate << endl;
    // cout << "Upload Cleanup " << Location.upload_cleanup[0] << endl;
    // cout << "Try Files " << Location.try_files[0] << endl;
    return 0;
}
// void    parseLocation(const std::string &filename)
// {
//     std::ifstream   file(filename.c_str());
//     std::string     line;
//     std::string     key, value;
//     LocationConfig  newLocation;
//     bool            inLocationBlock = false;

//     while (getline(file, line)) {
//         replaceTabsWithSpaces(line);
//         std::istringstream iss(line);
//         std::string key;
//         if (line.find("}") != std::string::npos && inLocationBlock) {
//             Server.locations.insert(std::pair<std::string, LocationConfig>(newLocation.path, newLocation));
//             inLocationBlock = false;
//         }
//         if (!(iss >> key)) continue; // Skip empty lines

//         if (key == "location") {
//             // 생성자 만들어줘야함 내부는
//             inLocationBlock = true;
//         }
//         if (key == "path") {
//             std::string path;
//             iss >> path;
//             if (path.empty())
//                 continue;
//             else
//                 newLocation.path = path;
//         } else if (key == "alias") {
//             std::string alias;
//             iss >> alias;
//             if (alias.empty())
//                 continue;
//             else
//                 newLocation.alias = alias;
//         } else if (key == "root") {
//             std::string root;
//             iss >> root;
//             if (root.empty())
//                 continue;
//             else
//                 newLocation.root = root;
//         } else if (key == "index") {
//             std::string index;
//             iss >> index;
//             if (index.empty())
//                 continue;
//             else
//                 newLocation.index = index;
//         } else if (key == "autoindex") {
//             std::string autoindex;
//             iss >> autoindex;
//             if (autoindex.empty())
//                 continue;
//             // else
//             //     newLocation.autoindex = autoindex; // bool init
//         } else if (key == "allow_methods") {
//             std::string allow_methods;
//             iss >> allow_methods;
//             if (allow_methods.empty())
//                 continue;
//             else
//                 newLocation.allow_methods.push_back(allow_methods);
//         } else if (key == "return_code") {
//             std::string return_code;
//             iss >> return_code;
//             if (return_code.empty())
//                 continue;
//             else
//                 newLocation.return_code = return_code;
//         } else if (key == "return_url") {
//             std::string return_url;
//             iss >> return_url;
//             if (return_url.empty())
//                 continue;
//             else
//                 newLocation.return_url = return_url;
//         } else if (key == "cgi_pass") {
//             std::string fastcgi_pass;
//             iss >> fastcgi_pass;
//             if (fastcgi_pass.empty())
//                 continue;
//             else
//                 newLocation.fastcgi_pass = fastcgi_pass;
//         } else if (key == "cgi_param") {
//             std::string fastcgi_param;
//             iss >> fastcgi_param;
//             if (fastcgi_param.empty())
//                 continue;
//             else
//                 newLocation.fastcgi_param = fastcgi_param;
//         } else if (key == "upload_pass") {
//             std::string upload_pass;
//             iss >> upload_pass;
//             if (upload_pass.empty())
//                 continue;
//             else
//                 newLocation.upload_pass = upload_pass;
//         } else if (key == "upload_store") {
//             std::string upload_store;
//             iss >> upload_store;
//             if (upload_store.empty())
//                 continue;
//             else
//                 newLocation.upload_store = upload_store;
//         } else if (key == "upload_state_store") {
//             std::string upload_state_store;
//             iss >> upload_state_store;
//             if (upload_state_store.empty())
//                 continue;
//             else
//                 newLocation.upload_state_store = upload_state_store;
//         } else if (key == "upload_resume") {
//             std::string upload_resume;
//             iss >> upload_resume;
//             if (upload_resume.empty())
//                 continue;
//             // else
//             //     newLocation.upload_resume = upload_resume; // bool init
//         } else if (key == "upload_max_file_size") {
//             std::string upload_max_file_size;
//             iss >> upload_max_file_size;
//             if (upload_max_file_size.empty())
//                 continue;
//             else
//                 newLocation.upload_max_file_size = upload_max_file_size;
//         } else if (key == "upload_limit_rate") {
//             std::string upload_limit_rate;
//             iss >> upload_limit_rate;
//             if (upload_limit_rate.empty())
//                 continue;
//             else
//                 newLocation.upload_limit_rate = upload_limit_rate;
//         } else if (key == "upload_cleanup") {
//             std::string upload_cleanup;
//             iss >> upload_cleanup;
//             if (upload_cleanup.empty())
//                 continue;
//             else
//                 newLocation.upload_cleanup.push_back(std::stoi(upload_cleanup));
//         } else if (key == "try_files") {
//             std::string try_files;
//             iss >> try_files;
//             if (try_files.empty())
//                 continue;
//             else
//                 newLocation.try_files.push_back(try_files);
//         }
//     }
//     Location = newLocation;
// }
