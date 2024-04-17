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


class Socket;

/* test config 클래스 구조 */

class Config {
public:
	std::unordered_map<FD, std::string> config;
	Config() {}
	~Config() {}
	Port getPort() { return 8888; }
	void dispose(const FD &Socket) {
		config.erase(Socket);
	}
	void registerSocket(const FD &socket) {
		config.insert(std::make_pair(socket, "socket"));
	}
	std::string getHost() { return ""; }
};

class Socket {
private:
    FD _ListenSocket;
	Config* _Conf;
	struct sockaddr_in _Server_Addr;
	
	void __init__();
	void __init__(Config *_Conf);
	void __init__(std::string host, Port port);


public:
    Socket();
    Socket(const Socket& other);
	Socket(Config *config);
	Socket(std::string host, Port port);
    ~Socket();

	Socket& operator=(const Socket& other);
	operator FD() const { return _ListenSocket; } // 타입 캐스팅 연산자 오버로딩
	operator FD&() { return _ListenSocket; } // 타입 캐스팅 연산자 오버로딩


    Socket& socket(int domain, int type, int protocol);
	Status bind(const std::string &host, const Port &port);
	Status listen(size_t backlog);
	Status close();
	FD accept() const;

	void setSocketOption(int level, int option_name, int opt);
	void __init__SocketoptAuto(int opt);

	void nonblocking();
	static void nonblocking(const FD &socket);

	FD autoActivate(Config *config, std::string host, Port port, int domain, int type, int protocol);

	Socket *clone() const;

};


/* Socket 기본설정 */

void Socket::__init__() {
    memset(&_Server_Addr, 0, sizeof(_Server_Addr)); // _Server_Addr 초기화
    _Server_Addr.sin_family = AF_INET; // IPv4
    _Server_Addr.sin_addr.s_addr = INADDR_ANY; // 모든 IP 주소로부터의 연결 허용 자동으로 호스트의 IP 주소를 찾아서 대입
    _Server_Addr.sin_port = htons(80); // 기본 포트 80 설정
}

void Socket::__init__(Config *config) {
    memset(&_Server_Addr, 0, sizeof(_Server_Addr)); // _Server_Addr 초기화
    _Server_Addr.sin_family = AF_INET; // IPv4
    _Server_Addr.sin_addr.s_addr = inet_addr(config->getHost().c_str()); // 모든 IP 주소로부터의 연결 허용 자동으로 호스트의 IP 주소를 찾아서 대입
    _Server_Addr.sin_port = htons(config->getPort()); // 기본 포트 80 설정
}

void Socket::__init__(std::string host, Port port) {
	memset(&_Server_Addr, 0, sizeof(_Server_Addr)); // _Server_Addr 초기화
	_Server_Addr.sin_family = AF_INET; // IPv4
	_Server_Addr.sin_addr.s_addr = inet_addr(host.c_str()); // 모든 IP 주소로부터의 연결 허용 자동으로 호스트의 IP 주소를 찾아서 대입
	_Server_Addr.sin_port = htons(port); // 기본 포트 80 설정
}

// Socket 클래스 생성자

Socket::Socket() : _ListenSocket(-1), _Conf(nullPtr) {
	__init__();
    std::cout << "Socket initialized" << std::endl;
}

Socket::Socket(std::string host, Port port) : _ListenSocket(-1), _Conf(nullPtr) {
	__init__(host, port);
	std::cout << "Socket initialized with host and port" << std::endl;
}

Socket::Socket(Config *config) : _ListenSocket(-1), _Conf(config) {
	if (config)
	{
		config->registerSocket(*this);
		__init__(config);
		std::cout << "Socket initialized with config" << std::endl;
	}
	else
	{
		__init__();
		std::cout << "Socket initialized but config is Null" << std::endl;
		std::cout << "Please set config" << std::endl;
	}
}



Socket::Socket(const Socket& other) : _ListenSocket(dup(other._ListenSocket)), _Conf(other._Conf), _Server_Addr(other._Server_Addr) {
    std::cout << "Socket copied" << std::endl;
}


/* Socket 생성 */
Socket& Socket::socket(int domain=AF_INET, int type=SOCK_STREAM, int protocol=0) {
	std::cout << "Socket created" << std::endl;
    _ListenSocket = ::socket(domain, type, protocol);
    if (_ListenSocket == -1) {
        std::cerr << "Error: Socket creation failed" << std::endl;
        throw std::runtime_error("Error: Socket creation failed");
    }
    return *this;
}

/* Socket 바인딩 */

Status Socket::bind(const std::string &host="", const Port &port=80) {
	if (!host.empty()) {
		_Server_Addr.sin_addr.s_addr = inet_addr(host.c_str());
	}
	else {
		_Server_Addr.sin_addr.s_addr = INADDR_ANY;
	}

	_Server_Addr.sin_port = htons(port);

	if (::bind(_ListenSocket, (struct sockaddr*)&_Server_Addr, sizeof(_Server_Addr)) == FAILURE) {
		std::cerr << "Error: Failed to bind socket" << std::endl;
		throw std::runtime_error("Error: Failed to bind socket");
	}
	return SUCCESS;
}

/* Socket 리스닝 */

Status Socket::listen(size_t backlog=5) {
	if (::listen(_ListenSocket, backlog) == FAILURE) {
		std::cerr << "Error: Failed to listen socket" << std::endl;
		throw std::runtime_error("Error: Failed to listen socket");
	}
	return SUCCESS;
}


FD Socket::accept() const {
    FD Client_Socket = -1;

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    Client_Socket = ::accept(_ListenSocket, (struct sockaddr*)&client_addr, &client_addr_size);
    if (Client_Socket == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 클라이언트 연결 요청이 없는 경우, 다른 작업을 수행하거나 잠시 대기할 수 있습니다.
            return Client_Socket;
        } else {
            // 실제 에러 발생 시 예외 처리합니다.
            std::cerr << "Error: Failed to accept socket. Error code: " << errno << std::endl;
            throw std::runtime_error("Error: Failed to accept socket");
        }
    }

    return Client_Socket;
}


/* Socket 닫기 */

Status Socket::close() {
	if (::close(_ListenSocket) == FAILURE) {
		std::cerr << "Error: Failed to close socket" << std::endl;
		throw std::runtime_error("Error: Failed to close socket");
	}
	memset(&_Server_Addr, 0, sizeof(_Server_Addr));
	_ListenSocket = -1;
	return SUCCESS;
}

/* Socket 비동기 활성화 */

void Socket::nonblocking() {
	int flags = fcntl(_ListenSocket, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error: Failed to get socket flags" << std::endl;
		return ;
	}

	flags |= O_NONBLOCK;
	if (fcntl(_ListenSocket, F_SETFL, flags) == -1) {
		std::cerr << "Error: Failed to set socket flags" << std::endl;
		return ;
	}
}

void Socket::nonblocking(const FD &socket) {
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error: Failed to get socket flags" << std::endl;
		return ;
	}

	flags |= O_NONBLOCK;
	if (fcntl(socket, F_SETFL, flags) == -1) {
		std::cerr << "Error: Failed to set socket flags" << std::endl;
		return ;
	}
}

/* Socket 옵션 설정 */

void Socket::setSocketOption(int level, int option_name, int opt=1) {
	socklen_t optlen = sizeof(opt);

	if (setsockopt(this->_ListenSocket, level, option_name, &opt, optlen) == FAILURE) {
		std::cerr << "Error: Failed to set socket option" << std::endl;
		throw std::runtime_error("Error: Failed to set socket option");
	}
}

void Socket::__init__SocketoptAuto(int opt=1) {
	setSocketOption(SOL_SOCKET, SO_REUSEADDR, opt); // 소켓 재사용 옵션 설정
	setSocketOption(SOL_SOCKET, SO_REUSEPORT, opt); // 포트 재사용 옵션 설정
	setSocketOption(SOL_SOCKET, SO_KEEPALIVE, opt); // keep-alive 옵션 설정 --> 연결이 끊어진 경우 자원을 반환하고 연결을 끊음 --> 리소스 절약
}

/* Socket 자동 활성화 */

FD Socket::autoActivate(Config *config=nullPtr, std::string host="", Port port=80, int domain=AF_INET, int type=SOCK_STREAM, int protocol=0) {
	socket(domain, type, protocol);
	__init__SocketoptAuto();
	bind(host, port);
	listen();
	return _ListenSocket;
}

/* Socket 복제 */

Socket *Socket::clone() const {
	return new Socket(*this);
}

/* Socket 소멸자 */

Socket::~Socket() {
	
	if (_ListenSocket == -1) {
		std::cerr << "Error: Socket not initialized" << std::endl;
	} else if (_ListenSocket == 0) {
		std::cerr << "Error: Socket not opened" << std::endl;
	}
	else {
		int status = ::close(_ListenSocket);
		if (status == -1) {
			std::cerr << "Error: Socket close failed" << std::endl;
		} else {
			std::cout << "Socket closed" << std::endl;
		}
	}
}

/* Socket 할당 해제 */

Socket& Socket::operator=(const Socket& other) {
    if (this != &other) {
        ::close(_ListenSocket);
		_Conf->dispose(*this); // 기존 소켓 제거
        this->_ListenSocket = dup(other._ListenSocket);
		this->_Server_Addr = other._Server_Addr;
		_Conf->registerSocket(*this); // 새로운 소켓 등록
    }
    return *this;
}



