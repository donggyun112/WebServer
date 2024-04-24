#include "LocationConfig.hpp"
void replaceTabsWithSpaces(std::string& str);

std::map<std::string, LocationConfig>    LocationConfig::parseLocation(const std::string &filename)
{
    std::ifstream   file(filename.c_str());
    std::string     line;
    std::string     key, value;
    LocationConfig  newLocation;
    std::map<std::string, LocationConfig> locations;
    bool            inServerBlock = false;
    bool            inLocationBlock = false;

    while (getline(file, line)) {
        replaceTabsWithSpaces(line);
        std::istringstream iss(line);
        std::string key;
        
        if (!(iss >> key)) continue; // Skip empty lines

        if (line == "server")   inServerBlock = true;
        if (key == "location")  inLocationBlock = true;
        
        if (line.find("}") != std::string::npos) {
            if (inLocationBlock && inServerBlock) {
                inLocationBlock = false;
            } else if (inLocationBlock == false) {
                inServerBlock = false;
            }
        }
        if (key == "path") {
            std::string path;
            iss >> path;
            if (path.empty())
                continue;
            else
                newLocation.setPath(path);
        } else if (key == "alias") {
            std::string alias;
            iss >> alias;
            if (alias.empty())
                continue;
            else
                newLocation.setAlias(alias);
        } else if (key == "root") {
            std::string root;
            iss >> root;
            if (root.empty())
                continue;
            else
                newLocation.setRoot(root);
        } else if (key == "index") {
            std::string index;
            iss >> index;
            if (index.empty())
                continue;
            else
                newLocation.setIndex(index);
    //     } else if (key == "autoindex") {
    //         std::string autoindex;
    //         iss >> autoindex;
    //         if (autoindex.empty())
    //             continue;
    //         // else
    //         //     newLocation.autoindex = autoindex; // bool init
    //     } else if (key == "allow_methods") {
    //         std::string allow_methods;
    //         iss >> allow_methods;
    //         if (allow_methods.empty())
    //             continue;
    //         // else
    //             // newLocation.setAllowMethods.push_back(allow_methods);
    //     } else if (key == "return_code") {
    //         std::string return_code;
    //         iss >> return_code;
    //         if (return_code.empty())
    //             continue;
    //         else
    //             newLocation.setReturnCode(return_code);
    //     } else if (key == "return_url") {
    //         std::string return_url;
    //         iss >> return_url;
    //         if (return_url.empty())
    //             continue;
    //         else
    //             newLocation.setReturnUrl(return_url);
    //     } else if (key == "cgi_pass") {
    //         std::string fastcgi_pass;
    //         iss >> fastcgi_pass;
    //         if (fastcgi_pass.empty())
    //             continue;
    //         else
    //             newLocation.setFastcgiPass(fastcgi_pass);
    //     } else if (key == "cgi_param") {
    //         std::string fastcgi_param;
    //         iss >> fastcgi_param;
    //         if (fastcgi_param.empty())
    //             continue;
    //         else
    //             newLocation.setFastcgiParam(fastcgi_param);
    //     } else if (key == "upload_pass") {
    //         std::string upload_pass;
    //         iss >> upload_pass;
    //         if (upload_pass.empty())
    //             continue;
    //         else
    //             newLocation.setUploadPass(upload_pass);
    //     } else if (key == "upload_store") {
    //         std::string upload_store;
    //         iss >> upload_store;
    //         if (upload_store.empty())
    //             continue;
    //         else
    //             newLocation.setUploadStore(upload_store);
    //     } else if (key == "upload_state_store") {
    //         std::string upload_state_store;
    //         iss >> upload_state_store;
    //         if (upload_state_store.empty())
    //             continue;
    //         else
    //             newLocation.setUploadStateStore(upload_state_store);
    //     } else if (key == "upload_resume") {
    //         std::string upload_resume;
    //         iss >> upload_resume;
    //         if (upload_resume.empty())
    //             continue;
    //         // else
    //         //     newLocation.upload_resume = upload_resume; // bool init
    //     } else if (key == "upload_max_file_size") {
    //         std::string upload_max_file_size;
    //         iss >> upload_max_file_size;
    //         if (upload_max_file_size.empty())
    //             continue;
    //         else
    //             newLocation.setUploadMaxFileSize(upload_max_file_size);
    //     } else if (key == "upload_limit_rate") {
    //         std::string upload_limit_rate;
    //         iss >> upload_limit_rate;
    //         if (upload_limit_rate.empty())
    //             continue;
    //         else
    //             newLocation.setUploadLimitRate(upload_limit_rate);
    //     } else if (key == "upload_cleanup") {
    //         std::string upload_cleanup;
    //         iss >> upload_cleanup;
    //         if (upload_cleanup.empty())
    //             continue;
    //         // else
    //         //     newLocation.setUploadCleanup(upload_cleanup);
    //     } else if (key == "try_files") {
    //         std::string try_files;
    //         iss >> try_files;
    //         if (try_files.empty())
    //             continue;
    //         // else
    //         //     newLocation.try_files.push_back(try_files);
    //     }
    // }
        }
    }
    return locations;
}

using namespace std;

int main()
{
    LocationConfig a;
    std::map<std::string, LocationConfig> locations = a.parseLocation("nginx.conf");

    cout << "start parseLocation" << endl;
    cout << locations["/"].getPath() << endl; // should print "/"

    return 0;
}