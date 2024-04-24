#include "LocationConfig.hpp"

std::string                 LocationConfig::getPath() const { return _path; }

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

// bool                        LocationConfig::getAutoindex() const {
//     return _autoindex;
// }

// std::vector<std::string>    LocationConfig::getAllowMethods() const {
//     return _allow_methods;
// }

// std::string                 LocationConfig::getReturnCode() const {
//     if (_return_code.empty())
//         return "200";
//     return _return_code;
// }

// std::string                 LocationConfig::getReturnUrl() const {
//     if (_return_url.empty())
//         return "/";
//     return _return_url;
// }

// std::string                 LocationConfig::getFastcgiPass() const {
//     if (_fastcgi_pass.empty())
//         return "";
//     return _fastcgi_pass;
// }

// std::string                 LocationConfig::getUploadPass() const {
//     if (_upload_pass.empty())
//         return "";
//     return _upload_pass;
// }

// std::string                 LocationConfig::getUploadStore() const {
//     if (_upload_store.empty())
//         return "";
//     return _upload_store;
// }

// std::string                 LocationConfig::getUploadStateStore() const {
//     if (_upload_state_store.empty())
//         return "";
//     return _upload_state_store;
// }

// bool                        LocationConfig::getUploadResume() const {
//     return _upload_resume;
// }

// std::string                 LocationConfig::getUploadMaxFileSize() const {
//     if (_upload_max_file_size.empty())
//         return "1m";
//     return _upload_max_file_size;
// }

// std::string                 LocationConfig::getUploadLimitRate() const {
//     if (_upload_limit_rate.empty())
//         return "0";
//     return _upload_limit_rate;
// }

// std::vector<int>            LocationConfig::getUploadCleanup() const {
//     return _upload_cleanup;
// }

// std::vector<std::string>    LocationConfig::getTryFiles() const {
//     return _try_files;
// }
