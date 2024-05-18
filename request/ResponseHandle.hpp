#pragma once

// #include "Response.hpp"
#include "RequestHandle.hpp"
#include "../Parse/Config.hpp"
#include "../utils/utils.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "../utils/Error.hpp"
#include <iostream>
#include <regex>

class Response;

class ResponseHandle {
	private:
		// Response 		_response;
		bool			_isInitFromLocation;
		std::string 	_response;
		std::string 	_filePath;
		std::string 	_pathInfo;
		std::string 	_scriptName;
		std::string 	_httpUri;
		std::string 	_serverRoot;
		Port			_port;
		LocationConfig	_loc;
	public:
		bool 			initPathFromLocation(const RequestHandle &Req, Config &Conf);
		ResponseHandle();
		ResponseHandle(const ResponseHandle &Copy);
		~ResponseHandle();
		std::string	generateHTTPFullString(const RequestHandle &Req, Config &Conf);
		std::string	handleGetRequest(const RequestHandle &Req, int maxBodySize);
		std::string handlePostRequest(const RequestHandle &Req);
		Response	handleMethodNotAllowed();
		std::string handleFormData(const std::string &cgiPath, const RequestHandle &Req);
		std::string getFilePath(const std::string &serverRoot, const std::string &httpUri, LocationConfig &loc);
		Response	createErrorResponse(int code, const std::string &message);
		Response 	handleRedirect(const LocationConfig &location);
		void		handleAutoIndex(Response &response, const std::string &servRoot);
		std::string	handleDeleteRequest();
		void		setResponse(const std::string &response);
		void		clearAll();
		const std::string getResponse();
		bool		isCGI() const;
		std::string	getFilePath() const;
		std::string	getPathInfo() const;
		std::string	getScriptName() const;
		std::string	getHttpUri() const;
		std::string	getServerRoot() const;
		Port		getPort() const;
		LocationConfig	getLocation() const;
		ResponseHandle& operator=(const ResponseHandle &Copy);
};

