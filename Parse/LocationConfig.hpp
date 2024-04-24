#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "Config.hpp"

class LocationConfig {
    private:
    friend class Config;
    friend class ServerConfig;
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
    std::unordered_map<int, std::string> parseErrorPages(std::istringstream &iss);
    static LocationConfig    parseLocation(std::ifstream &file);
    // setter
    void    setPath(const std::string &path)
{
    this->_path = path;
}

void    setAlias(const std::string &alias)
{
    this->_alias = alias;
}

void    setRoot(const std::string &root)
{
    this->_root = root;
}

void    setIndex(const std::string &index)
{
    this->_index = index;
}

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
std::string                 getPath() { return _path; }

std::string                 getAlias() { return _alias; }

std::string                 getRoot() {
    if (_root.empty())
        return ".";
    return _root;
}

std::string                 getIndex() {
    if (_index.empty())
        return "index.html";
    return _index;
}
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
void replaceTabsWithSpaces(std::string& str);


#endif