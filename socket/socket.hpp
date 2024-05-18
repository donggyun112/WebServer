#ifndef SOCKET_HPP
# define SOCKET_HPP

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
#include <netdb.h>

class Socket {
private:
	FD _listenSocket;
	Port _port;
	std::string _host;
	struct addrinfo *_addrInfo;

	void __init__(std::string host, Port port);
	void __init__(Port port);
	Socket();

public:
	Socket(const Socket& other);
	Socket(std::string host, Port port);
	~Socket();

	Socket& operator=(const Socket& other);
	operator FD() const { return _listenSocket; }
	operator FD&() { return _listenSocket; }

	FD socket(int domain, int type, int protocol);
	Status bind();
	Status listen(size_t backlog);
	Status close();
	static	FD accept(FD _listenSocket);

	void			__init__SocketoptAuto(int opt);

	Status 			nonblocking();
	static Status 	nonblocking(const FD &socket);

	void 			autoActivate(int domain=AF_INET, int type=SOCK_STREAM, int protocol=0);

	Socket 			*clone() const;

	void 			setSocketOption(int level, int option_name, int opt);
	void 			setAddrInfo(struct addrinfo *addrInfo);
	void 			setSocket(FD socket) { _listenSocket = socket; }

	FD 				getSocket() const { return _listenSocket; }
	struct addrinfo *getAddrInfo() const { return _addrInfo; }
	Port 			getPort() const { return _port; }
	std::string 	getHost() const { return _host; }
	std::string 	getSocketIP() const;
	Port			getSocketPort() const;
	std::string		getProtocolName() const;
	static int		getSocketOPT(const FD &socket);
};
#endif