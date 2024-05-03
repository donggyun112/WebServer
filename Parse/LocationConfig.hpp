#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

class LocationConfig {
    private:
    // std::unordered_map<std::string, std::string> fastcgi_params;
    std::string                 _path;
    std::string                 _alias;
    std::string                 _root;
    std::string                 _index;
    bool                        _autoindex;
    std::vector<std::string>    _allow_methods;
    std::string                 _return_code;
    std::string                 _return_url;
    std::string                 _fastcgi_pass;
    std::string                 _fastcgi_param;
    std::string                 _upload_pass;
    std::string                 _upload_store;
    std::string                 _upload_state_store;
    bool                        _upload_resume;
    std::string                 _upload_max_file_size;
    std::string                 _upload_limit_rate;
    std::vector<int>            _upload_cleanup;
	std::vector<std::string>    _try_files;
     
    public:
    ~LocationConfig() {}
	LocationConfig() {}
    // setter
    LocationConfig(std::ifstream &file);
    
    // getter
    std::string                 getPath() const ;
    std::string                 getAlias() const ;
    std::string                 getRoot() const ;
    std::string                 getIndex() const ;
    bool                        getAutoindex() const ;
    std::vector<std::string>    getAllowMethods() const ;
    std::string                 getReturnCode() const ;
    std::string                 getReturnUrl() const ;
    std::string                 getFastcgiPass() const ;
    std::string                 getUploadPass() const ;
    std::string                 getUploadStore() const ;
    std::string                 getUploadStateStore() const ;
    bool                        getUploadResume() const ;
    std::string                 getUploadMaxFileSize() const ;
    std::string                 getUploadLimitRate() const ;
    std::vector<int>            getUploadCleanup() const ;
    std::vector<std::string>    getTryFiles() const ;
    // // 추가적인 필요한 변수들
};
#endif