#include "LocationConfig.hpp"
#include "Config.hpp"

LocationConfig::LocationConfig(std::ifstream &file)
{
    std::string     line;
    std::string     key, value;

    while (std::getline(file, line)) {
        if (line[0] == '}') {
            file.seekg(-(line.length() + 1), std::ios::cur);
            break;
        }
        replaceTabsWithSpaces(line);
        std::istringstream iss(line);
        std::string key;
        
        if (!(iss >> key)) continue; // Skip empty lines
        
        if (key == "}") {
            break ;
        }
    
        if (key == "location") {
            std::string path;
            iss >> path;
            if (path.empty())
                continue;
            else
                this->_path = path;
        } else if (key == "alias") {
            std::string alias;
            iss >> alias;
            if (alias.empty())
                continue;
            else
                this->_alias = alias;
        } else if (key == "root") {
            std::string root;
            iss >> root;
            if (root.empty())
                continue;
            else
                this->_root = root;
        } else if (key == "index") {
            std::string index;
            iss >> index;
            if (index.empty())
                continue;
            else
                this->_index = index;
    //     } else if (key == "autoindex") {
    //         std::string autoindex;
    //         iss >> autoindex;
    //         if (autoindex.empty())
    //             continue;
    //         // else
    //         //     this->autoindex = autoindex; // bool init
    //     } else if (key == "allow_methods") {
    //         std::string allow_methods;
    //         iss >> allow_methods;
    //         if (allow_methods.empty())
    //             continue;
    //         // else
    //             // this->setAllowMethods.push_back(allow_methods);
    //     } else if (key == "return_code") {
    //         std::string return_code;
    //         iss >> return_code;
    //         if (return_code.empty())
    //             continue;
    //         else
    //             this->setReturnCode(return_code);
    //     } else if (key == "return_url") {
    //         std::string return_url;
    //         iss >> return_url;
    //         if (return_url.empty())
    //             continue;
    //         else
    //             this->setReturnUrl(return_url);
    //     } else if (key == "cgi_pass") {
    //         std::string fastcgi_pass;
    //         iss >> fastcgi_pass;
    //         if (fastcgi_pass.empty())
    //             continue;
    //         else
    //             this->setFastcgiPass(fastcgi_pass);
    //     } else if (key == "cgi_param") {
    //         std::string fastcgi_param;
    //         iss >> fastcgi_param;
    //         if (fastcgi_param.empty())
    //             continue;
    //         else
    //             this->setFastcgiParam(fastcgi_param);
    //     } else if (key == "upload_pass") {
    //         std::string upload_pass;
    //         iss >> upload_pass;
    //         if (upload_pass.empty())
    //             continue;
    //         else
    //             this->setUploadPass(upload_pass);
    //     } else if (key == "upload_store") {
    //         std::string upload_store;
    //         iss >> upload_store;
    //         if (upload_store.empty())
    //             continue;
    //         else
    //             this->setUploadStore(upload_store);
    //     } else if (key == "upload_state_store") {
    //         std::string upload_state_store;
    //         iss >> upload_state_store;
    //         if (upload_state_store.empty())
    //             continue;
    //         else
    //             this->setUploadStateStore(upload_state_store);
    //     } else if (key == "upload_resume") {
    //         std::string upload_resume;
    //         iss >> upload_resume;
    //         if (upload_resume.empty())
    //             continue;
    //         // else
    //         //     this->upload_resume = upload_resume; // bool init
    //     } else if (key == "upload_max_file_size") {
    //         std::string upload_max_file_size;
    //         iss >> upload_max_file_size;
    //         if (upload_max_file_size.empty())
    //             continue;
    //         else
    //             this->setUploadMaxFileSize(upload_max_file_size);
    //     } else if (key == "upload_limit_rate") {
    //         std::string upload_limit_rate;
    //         iss >> upload_limit_rate;
    //         if (upload_limit_rate.empty())
    //             continue;
    //         else
    //             this->setUploadLimitRate(upload_limit_rate);
    //     } else if (key == "upload_cleanup") {
    //         std::string upload_cleanup;
    //         iss >> upload_cleanup;
    //         if (upload_cleanup.empty())
    //             continue;
    //         // else
    //         //     this->setUploadCleanup(upload_cleanup);
    //     } else if (key == "try_files") {
    //         std::string try_files;
    //         iss >> try_files;
    //         if (try_files.empty())
    //             continue;
    //         // else
    //         //     this->try_files.push_back(try_files);
    //     }
    // }
        }
    }
}

std::string                 LocationConfig::getPath() const { return this->_path; }

std::string                 LocationConfig::getAlias() const { return _alias; }

std::string                 LocationConfig::getRoot() const {
    if (_root.empty())
        return ".";
    return _root;
}

std::string                 LocationConfig::getIndex() const {
    if (_index.empty())
        return "index.html";
    return _index;
}

bool                        LocationConfig::getAutoindex() const {
    return _autoindex;
}

std::vector<std::string>    LocationConfig::getAllowMethods() const {
    return _allow_methods;
}

std::string                 LocationConfig::getReturnCode() const {
    return _return_code;
}

std::string                 LocationConfig::getReturnUrl() const {
    return _return_url;
}

std::string                 LocationConfig::getFastcgiPass() const {
    if (_fastcgi_pass.empty())
        return "";
    return _fastcgi_pass;
}

std::string                 LocationConfig::getUploadPass() const {
    if (_upload_pass.empty())
        return "";
    return _upload_pass;
}

std::string                 LocationConfig::getUploadStore() const {
    if (_upload_store.empty())
        return "";
    return _upload_store;
}

std::string                 LocationConfig::getUploadStateStore() const {
    if (_upload_state_store.empty())
        return "";
    return _upload_state_store;
}

bool                        LocationConfig::getUploadResume() const {
    return _upload_resume;
}

std::string                 LocationConfig::getUploadMaxFileSize() const {
    if (_upload_max_file_size.empty())
        return "1m";
    return _upload_max_file_size;
}

std::string                 LocationConfig::getUploadLimitRate() const {
    if (_upload_limit_rate.empty())
        return "0";
    return _upload_limit_rate;
}

std::vector<int>            LocationConfig::getUploadCleanup() const {
    return _upload_cleanup;
}

std::vector<std::string>    LocationConfig::getTryFiles() const {
    return _try_files;
}
