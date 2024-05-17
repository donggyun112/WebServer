#ifndef NRESPONSEUTILS_HPP
# define NRESPONSEUTILS_HPP
# include "../Parse/Config.hpp"
# include <sys/stat.h>

namespace ResponseUtils {
	std::string nomralizeUrl(const std::string &HTTP_uri);
	std::string normalizePath(const std::string &path);
	bool		isMethodPossible(int method, const LocationConfig &Loc);
	bool		isValidPath(const std::string &path);
	bool		isDirectory(const std::string &path);
	bool		isFile(const std::string &path);
	bool		isExist(const std::string &path);
	bool		isExtention(std::string httpPath);
	std::string readFileContent(std::ifstream &file, std::streamsize fileSize);
	std::streamsize getFileSize(std::ifstream &file);
	std::string getCurTime();
	
	std::string getContentType(const std::string &extension);
	int			getMethodNumber(const std::string &method);
	std::string	getFileExtension(const std::string &filePath);
	std::string getFormattedTime(time_t time);
	std::string getFormatSize(double size);
	std::string getLastModified(const std::string& filePath);
    std::string generateETag(const std::string& content);
}
#endif