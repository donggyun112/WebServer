#pragma once

#include <iostream>
#include "RequestHandle.hpp"
#include "ResponseHandle.hpp"
#include "../utils/Error.hpp"

class Client {
	private:
		Port 			_port;
		RequestHandle	_requestHandle;
		ResponseHandle	_responseHandle;
		std::string		_response;
		Client();
	public:
		Client(Port port);
		Client(const Client &Copy);
		~Client();
		void clearAll();
		void setBuffer(const std::string &buffer);
		int  getReadStatus() const;
		void generateResponse(Config Conf);
		Port getPort() const;
		// std::string setResponse();
		std::string getResponse() const;
};
