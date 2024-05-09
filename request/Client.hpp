#pragma once

#include <iostream>
#include "RequestHandle.hpp"

class Client {
	private:
		Port _port;
		RequestHandle _requestHandle;
		std::string _response;
		Client();
	public:
		Client(Port port);
		Client(const Client &Copy);
		~Client();
		void clearAll();
		void setBuffer(const std::string &buffer);
		int  getReadStatus() const;
		void executeRequest(const Config &Conf);
		const std::string getResponse() const;
};
