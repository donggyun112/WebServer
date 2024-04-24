#pragma once

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unordered_map>
#include <string>
#include <fcntl.h>
#include "../utils/utils.hpp"

class Socket {
private:
    FD _listenSocket;
	Port _port;
	std::string _host;
	struct sockaddr_in _server_Addr;
	
	void __init__();
	void __init__(std::string host, Port port);
	void __init__(Port port);


public:
    Socket();
    Socket(const Socket& other);
	Socket(std::string host, Port port);
    ~Socket();

	Socket& operator=(const Socket& other);
	operator FD() const { return _listenSocket; } // 타입 캐스팅 연산자 오버로딩
	operator FD&() { return _listenSocket; } // 타입 캐스팅 연산자 오버로딩


    FD socket(int domain, int type, int protocol);
	Status bind(const std::string host, const Port port);
	Status listen(size_t backlog);
	Status close();
	FD accept() const;

	void setSocketOption(int level, int option_name, int opt);
	void __init__SocketoptAuto(int opt);

	Status nonblocking();
	static Status nonblocking(const FD &socket);

	void autoActivate(int domain=AF_INET, int type=SOCK_STREAM, int protocol=0);

	Socket *clone() const;

	void setServerAddr(struct sockaddr_in serverAddr) { _server_Addr = serverAddr; }
	void setSocket(FD socket) { _listenSocket = socket; }

	FD getSocket() const { return _listenSocket; }
	struct sockaddr_in getServerAddr() const { return _server_Addr; }
	Port getPort() const { return _port; }
	std::string getHost() const { return _host; }

};
