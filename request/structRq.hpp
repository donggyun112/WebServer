#ifndef STRUCTRQ_HPP
#define STRUCTRQ_HPP

#include <map>

typedef std::map<std::string, std::string> Headers;
typedef std::map<std::string, std::string> Cookies;

struct Request{
    std::string _method;
    std::string _uri;
    std::string _version;
    Headers     _headers;
    Cookies     _cookie;
    std::string _body;
    int         _contentLength;
};

#endif