#include "NResponseUtils.hpp"

bool    ResponseUtils::isMethodPossible(int method, const LocationConfig &Loc) {
    for (size_t i = 0; i < Loc.getAllowMethods().size(); ++i) {
        if (method == ResponseUtils::getMethodNumber(Loc.getAllowedMethod(i))) {
            return true;
        }
    }
    return false;
}

std::string ResponseUtils::getCurTime() {
	struct timeval tv;
	struct tm *tm;
	char buf[64];

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm);
	return std::string(buf);
}


int ResponseUtils::getMethodNumber(const std::string &method) {
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	else if (method == "PUT")
		return PUT;
	else
		return 0;
}

std::string ResponseUtils::nomralizeUrl(const std::string &HTTP_uri) {
	std::string normalizedUrl = HTTP_uri;
	
	// //제거
	std::string::size_type pos = 0;
	while ((pos = normalizedUrl.find("//", pos)) != std::string::npos) {
		normalizedUrl.erase(pos, 1);
	}
	return normalizedUrl;	
}

std::string ResponseUtils::normalizePath(const std::string &path) {
    std::string normalizedPath = path;
    
    // 연속된 슬래시를 하나의 슬래시로 치환
    std::string::size_type pos = 0;
    while ((pos = normalizedPath.find("//", pos)) != std::string::npos) {
        normalizedPath.erase(pos, 1);
    }
    
    // 경로의 마지막 문자가 '/'인 경우 제거
    if (!normalizedPath.empty() && normalizedPath[normalizedPath.length() - 1] == '/') {
        normalizedPath.erase(normalizedPath.length() - 1);
    }
    
    // 상대경로 요소 제거
    while ((pos = normalizedPath.find("/../")) != std::string::npos) {
        if (pos == 0) {
            return "";
        }
        std::string::size_type prevPos = normalizedPath.rfind('/', pos - 1);
        if (prevPos == std::string::npos) {
            return "";
        }
        normalizedPath.erase(prevPos, pos - prevPos + 3);
    }
    
    // 현재 디렉토리 요소 제거
    while ((pos = normalizedPath.find("/./")) != std::string::npos) {
        normalizedPath.erase(pos, 2);
    }
    
    return normalizedPath;
}
