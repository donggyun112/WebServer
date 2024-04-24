// #include "Config.hpp"
#include "ServerConfig.hpp"
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
    LocationConfig();
    ~LocationConfig();
    LocationConfig(const LocationConfig &other);
    // LocationConfig &operator=(const LocationConfig &other);
    std::map<std::string, LocationConfig> parseLocation(const std::string &filename);

    // setter
    void    setPath(const std::string &path);
    void    setAlias(const std::string &alias);
    void    setRoot(const std::string &root);
    void    setIndex(const std::string &index);
    // void    setAutoindex(const std::string &autoindex);
    // void    setAllowMethods(const std::string &allow_methods);
    // void    setReturnCode(const std::string &return_code);
    // void    setReturnUrl(const std::string &return_url);
    // void    setFastcgiPass(const std::string &fastcgi_pass);\
    // void    setFastcgiParam(const std::string &fastcgi_param);
    // void    setUploadPass(const std::string &upload_pass);
    // void    setUploadStore(const std::string &upload_store);
    // void    setUploadStateStore(const std::string &upload_state_store);
    // void    setUploadResume(const std::string &upload_resume);
    // void    setUploadMaxFileSize(const std::string &upload_max_file_size);
    // // void    setUploadLimitRate(const std::string &upload_limit_rate);
    // void    setUploadCleanup(const std::vector<int> &upload_cleanup);
    // void    setTryFiles(const std::vector<std::string> &try_files);

    // getter
    std::string                 getPath() const;
    std::string                 getAlias() const;
    std::string                 getRoot() const;
    std::string                 getIndex() const;
    // bool                        getAutoindex() const;
    // std::vector<std::string>    getAllowMethods() const;
    // std::string                 getReturnCode() const;
    // std::string                 getReturnUrl() const;
    // std::string                 getFastcgiPass() const;
    // std::string                 getUploadPass() const;
    // std::string                 getUploadStore() const;
    // std::string                 getUploadStateStore() const;
    // bool                        getUploadResume() const;
    // std::string                 getUploadMaxFileSize() const;
    // std::string                 getUploadLimitRate() const;
    // std::vector<int>            getUploadCleanup() const;
    // std::vector<std::string>    getTryFiles() const;
    // // 추가적인 필요한 변수들
};