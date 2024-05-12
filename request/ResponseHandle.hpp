#pragma once

// #include "Response.hpp"
#include "RequestHandle.hpp"
#include "../Parse/Config.hpp"
#include "../utils/utils.hpp"
#include "Response.hpp"
#include <iostream>


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
	std::string getFilePath(const std::string &serverRoot, const std::string &httpUri, const LocationConfig &loc);
	std::string getContentType(const std::string &extension);
	int			getMethodNumber(const std::string &method);
	std::string	getFileExtension(const std::string &filePath);
	std::string getFormattedTime(time_t time);
	std::string getFormatSize(double size);
}

class ResponseHandle {
	private:
		Response 		_response;
		std::string 	_filePath;
		std::string 	_httpUri;
		std::string 	_serverRoot;
		Port			_port;
		LocationConfig	_loc;
		bool 			initPathFromLocation(const RequestHandle &Req, Config &Conf);
	public:
		ResponseHandle();
		ResponseHandle(const ResponseHandle &Copy);
		~ResponseHandle();
		void		generateResponse(const RequestHandle &Req, Config &Conf);
		Response	handleGetRequest();
		Response	handleMethodNotAllowed();
		Response	createErrorResponse(int code, const std::string &message);
		Response 	handleRedirect(const LocationConfig &location);
		void		handleAutoIndex(Response &response, const std::string &servRoot);
		Response	handleDeleteRequest();
		void		clearAll();
		const std::string getResponse();
};

