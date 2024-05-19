#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include <unistd.h>
#include "RequestHandle.hpp"
#include "ResponseHandle.hpp"
#include "../utils/Error.hpp"
#include "../utils/utils.hpp"
#include "../Manager/Manager.hpp"
#include "../socket/socket.hpp"

#define READ 0
#define WRITE 1

typedef struct s_procInfo {
    pid_t pid;
    int pipeFd;
    int ClientFd;
} procInfo;

class Client {
	private:
		Port 			_port;
		RequestHandle	_requestHandle;
		ResponseHandle	_responseHandle;
		std::string		_response;
		procInfo 		*_procPtr;
		
		Client();
	public:
		Client(Port port);
		Client(const Client &Copy);
		~Client();
		void clearAll();
		void setBuffer(const std::string &buffer);
		int  getReadStatus() const;
		bool getIsKeepAlive() const;
		void generateResponse(Config Conf, char **env);
		void handleCGI(char **env);
		void setEnv(const RequestHandle &Req);
		void setBufferFromChild(int data);
		procInfo *getProcInfo() const;
		ResponseHandle getResponseHandle() const;



		// std::string setResponse();
		std::string getResponse() const;
		void		cutResponse(int length);
};
#endif