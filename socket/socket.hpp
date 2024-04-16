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


typedef int FD;
typedef int Port;

enum Status { SUCCESS=0, FAILURE=-1 };

class NullPtr {
public:
    NullPtr() {} // public 생성자

    // 임의의 타입으로 변환하는 operator
    template<typename T>
    operator T*() const
    {
        return NULL;
    }

    // 임의의 클래스 멤버 포인터로 변환하는 operator
    template<typename C, typename T>
    operator T C::*() const
    {
        return NULL;
    }

private:
    // NullPtr의 주소를 가져올 수 없도록 하는 private 멤버 함수
    void operator&() const;
};

NullPtr nullPtr; // 전역 객체 정의

class Socket;



class Config {
public:
	std::unordered_map<FD, std::string> config;
	Config(Socket &) {}
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
    FD server_socket;
	Config* config;
	struct sockaddr_in server_addr;
	
	void __init__();
	void __init__(Config *config);


public:
    Socket();
    Socket(const Socket& other);
	Socket(Config *config);
    ~Socket();

	Socket& operator=(const Socket& other);
	operator FD() const { return server_socket; } // 타입 캐스팅 연산자 오버로딩
	operator FD&() { return server_socket; } // 타입 캐스팅 연산자 오버로딩


    Socket& socket(int domain, int type, int protocol);
	Status bind(const std::string &host, const Port &port);
	Status listen(size_t backlog);
	Status close();
	std::pair<FD, struct sockaddr_in> accept();

	void setSocketOption(int level, int option_name, int opt);
	void __init__socketopt_auto(int opt);

	void nonblocking();
	static void nonblocking(const FD &socket);

};

// Socket 클래스 생성자

Socket::Socket() : server_socket(-1), config(nullPtr) {
	__init__();
    std::cout << "Socket initialized" << std::endl;
}

Socket::Socket(Config *config) : server_socket(-1), config(config) {
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


Socket::Socket(const Socket& other) : server_socket(dup(other.server_socket)), config(other.config), server_addr(other.server_addr) {
    std::cout << "Socket copied" << std::endl;
}

/* Socket 생성 */
Socket& Socket::socket(int domain=AF_INET, int type=SOCK_STREAM, int protocol=0) {
	std::cout << "Socket created" << std::endl;
    server_socket = ::socket(domain, type, protocol);
    if (server_socket == -1) {
        std::cerr << "Error: Socket creation failed" << std::endl;
        throw std::runtime_error("Error: Socket creation failed");
    }
    return *this;
}

/* Socket 바인딩 */

Status Socket::bind(const std::string &host="", const Port &port=80) {
	if (!host.empty()) {
		server_addr.sin_addr.s_addr = inet_addr(host.c_str());
	}
	else {
		server_addr.sin_addr.s_addr = INADDR_ANY;
	}

	server_addr.sin_port = htons(port);

	if (::bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FAILURE) {
		std::cerr << "Error: Failed to bind socket" << std::endl;
		throw std::runtime_error("Error: Failed to bind socket");
	}
	return SUCCESS;
}

/* Socket 리스닝 */

Status Socket::listen(size_t backlog=5) {
	if (::listen(server_socket, backlog) == FAILURE) {
		std::cerr << "Error: Failed to listen socket" << std::endl;
		throw std::runtime_error("Error: Failed to listen socket");
	}
	return SUCCESS;
}

/* Socket 연결 수락 */

std::pair<FD, struct sockaddr_in> Socket::accept() {
    FD client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    client_socket = ::accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_socket == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 클라이언트 연결 요청이 없는 경우, 다른 작업을 수행하거나 잠시 대기할 수 있습니다.
            return std::make_pair(-1, client_addr);
        } else {
            // 실제 에러 발생 시 예외 처리합니다.
            std::cerr << "Error: Failed to accept socket. Error code: " << errno << std::endl;
            throw std::runtime_error("Error: Failed to accept socket");
        }
    }

    return std::make_pair(client_socket, client_addr);
}

/* Socket 닫기 */

Status Socket::close() {
	if (::close(server_socket) == FAILURE) {
		std::cerr << "Error: Failed to close socket" << std::endl;
		throw std::runtime_error("Error: Failed to close socket");
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_socket = -1;
	return SUCCESS;
}

/* Socket 비동기 활성화 */

void Socket::nonblocking() {
	int flags = fcntl(server_socket, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error: Failed to get socket flags" << std::endl;
		return ;
	}

	flags |= O_NONBLOCK;
	if (fcntl(server_socket, F_SETFL, flags) == -1) {
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

	if (setsockopt(this->server_socket, level, option_name, &opt, optlen) == FAILURE) {
		std::cerr << "Error: Failed to set socket option" << std::endl;
		throw std::runtime_error("Error: Failed to set socket option");
	}
}


/* Socket 기본설정 */

void Socket::__init__() {
    memset(&server_addr, 0, sizeof(server_addr)); // server_addr 초기화
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // 모든 IP 주소로부터의 연결 허용 자동으로 호스트의 IP 주소를 찾아서 대입
    server_addr.sin_port = htons(80); // 기본 포트 80 설정
}

void Socket::__init__(Config *config) {
    memset(&server_addr, 0, sizeof(server_addr)); // server_addr 초기화
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr(config->getHost().c_str()); // 모든 IP 주소로부터의 연결 허용 자동으로 호스트의 IP 주소를 찾아서 대입
    server_addr.sin_port = htons(config->getPort()); // 기본 포트 80 설정
}

/* Socket 소멸자 */

Socket::~Socket() {
	
	if (server_socket == -1) {
		std::cerr << "Error: Socket not initialized" << std::endl;
	} else if (server_socket == 0) {
		std::cerr << "Error: Socket not opened" << std::endl;
	}
	else {
		int status = ::close(server_socket);
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
        ::close(server_socket);
		config->dispose(*this); // 기존 소켓 제거
        this->server_socket = dup(other.server_socket);
		this->server_addr = other.server_addr;
		config->registerSocket(*this); // 새로운 소켓 등록
    }
    return *this;
}

void Socket::__init__socketopt_auto(int opt=1) {
	setSocketOption(SOL_SOCKET, SO_REUSEADDR, opt); // 소켓 재사용 옵션 설정
	setSocketOption(SOL_SOCKET, SO_REUSEPORT, opt); // 포트 재사용 옵션 설정
	setSocketOption(SOL_SOCKET, SO_KEEPALIVE, opt); // keep-alive 옵션 설정 --> 연결이 끊어진 경우 자원을 반환하고 연결을 끊음 --> 리소스 절약
}
