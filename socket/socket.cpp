#include "socket.hpp"

/* Socket 기본설정 */

void Socket::__init__() {
    memset(&_server_Addr, 0, sizeof(_server_Addr)); // _server_Addr 초기화
    _server_Addr.sin_family = AF_INET; // IPv4
    _server_Addr.sin_addr.s_addr = INADDR_ANY; // 모든 IP 주소로부터의 연결 허용
    _server_Addr.sin_port = htons(8888); // 기본 포트 8888 설정
}

void Socket::__init__(std::string host, Port port) {
	memset(&_server_Addr, 0, sizeof(_server_Addr)); // _server_Addr 초기화
	_server_Addr.sin_family = AF_INET; // IPv4
	_server_Addr.sin_addr.s_addr = inet_addr(host.c_str()); // 모든 IP 주소로부터의 연결 허용 자동으로 호스트의 IP 주소를 찾아서 대입
	_server_Addr.sin_port = htons(port); // 기본 포트 8888 설정
	std::cout << _server_Addr.sin_port << std::endl;
}

void Socket::__init__(Port port) {
	memset(&_server_Addr, 0, sizeof(_server_Addr)); // _server_Addr 초기화
	_server_Addr.sin_family = AF_INET; // IPv4
	_server_Addr.sin_addr.s_addr = INADDR_ANY;
	_server_Addr.sin_port = htons(port); // 기본 포트 8888 설정
	std::cout << _server_Addr.sin_port << std::endl;
}

// Socket 클래스 생성자

Socket::Socket() : _listenSocket(-1), _port(8888), _host("127.0.0.1") {
	__init__();
    std::cout << "Socket initialized" << std::endl;
}

Socket::Socket(std::string host, Port port) : _listenSocket(-1), _port(port), _host(host) {
	if (host == "") {
		__init__(port);
	}
	else {
		__init__(host, port);
	}
	std::cout << "Socket initialized with host and port" << std::endl;
}


Socket::Socket(const Socket& other) : _server_Addr(other._server_Addr), _port(other._port), _host(other._host) {
	if (other._listenSocket != -1) {
		_listenSocket = dup(other._listenSocket);
	}
    // if (_listenSocket == -1) {
    //     throw std::runtime_error("Error: Failed to duplicate socket");
    // }
    std::cout << "Socket copied" << std::endl;
}


/* Socket 생성 */
FD Socket::socket(int domain=AF_INET, int type=SOCK_STREAM, int protocol=0) {
	std::cout << "Socket created" << std::endl;
    _listenSocket = ::socket(domain, type, protocol);
    if (_listenSocket == -1) {
        std::cerr << "Error: Socket creation failed. Error code: " << errno << std::endl;
        throw std::runtime_error("Error: Socket creation failed");
    }
    return _listenSocket;
}

/* Socket 바인딩 */

Status Socket::bind(const std::string host="127.0.0.1", const Port port=8888) {
	if (_server_Addr.sin_addr.s_addr != INADDR_ANY)
	{
		std::cout << "host: " << host << " port: " << port << std::endl;
		if (_server_Addr.sin_addr.s_addr != inet_addr(host.c_str())) {
			_server_Addr.sin_addr.s_addr = inet_addr(host.c_str());
			_host = host;
		}
		if (_server_Addr.sin_port != htons(port)) {
			_server_Addr.sin_port = htons(port);
			_port = port;
		}
	}
	else
	{
		std::cout << "host: " << "INADDR_ANY" << " port: " << port << std::endl;
		std::cout << "Server cannot be bound to a specific host" << std::endl;
	}
	

	if (::bind(_listenSocket, (struct sockaddr*)&_server_Addr, sizeof(_server_Addr)) == FAILURE) {
		std::cerr << "Error: Failed to bind socket. Error code: " << errno << std::endl;
		
		{
			_port = 0;
			_host = "127.0.0.1";
			_server_Addr.sin_port = htons(8888);
			_server_Addr.sin_addr.s_addr = INADDR_ANY;

		}

		throw std::runtime_error("Error: Failed to bind socket");
	}
	return SUCCESS;
}

/* Socket 리스닝 */

Status Socket::listen(size_t backlog=5) {
	if (::listen(_listenSocket, backlog) == FAILURE) {
		std::cerr << "Error: Failed to listen socket. Error code: " << errno << std::endl;
		throw std::runtime_error("Error: Failed to listen socket");
	}
	return SUCCESS;
}


FD Socket::accept() const {
    FD Client_Socket = -1;

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    Client_Socket = ::accept(_listenSocket, (struct sockaddr*)&client_addr, &client_addr_size);
	// std::cout << "Client_Socket : " << Client_Socket << std::endl;
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
	if (_listenSocket == -1) {
		std::cout << "Socket already closed" << std::endl;
		return SUCCESS;
	}
	
	if (::close(_listenSocket) == FAILURE) {
		std::cerr << "Error: Failed to close socket. Error code: " << errno << std::endl;
		throw std::runtime_error("Error: Failed to close socket");
	}
	memset(&_server_Addr, 0, sizeof(_server_Addr));
	_listenSocket = -1;
	return SUCCESS;
}

/* Socket 비동기 활성화 */

Status Socket::nonblocking() {
	int flags = fcntl(_listenSocket, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error: Failed to get socket flags. Error code: " << errno << std::endl;
		return FAILURE;
	}

	flags |= O_NONBLOCK;
	if (fcntl(_listenSocket, F_SETFL, flags) == -1) {
		std::cerr << "Error: Failed to set socket flags. Error code: " << errno << std::endl;
		return FAILURE;
	}
	return SUCCESS;
}

Status Socket::nonblocking(const FD &socket) {
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error: Failed to get socket flags. Error code: " << errno << std::endl;
		return FAILURE;
	}

	flags |= O_NONBLOCK;
	if (fcntl(socket, F_SETFL, flags) == -1) {
		std::cerr << "Error: Failed to set socket flags. Error code: " << errno << std::endl;
		return FAILURE;
	}
	return SUCCESS;
}

/* Socket 옵션 설정 */

void Socket::setSocketOption(int level, int option_name, int opt=1) {
	socklen_t optlen = sizeof(opt);

	if (setsockopt(this->_listenSocket, level, option_name, &opt, optlen) == FAILURE) {
		std::cerr << "Error: Failed to set socket option. Error code: " << errno << std::endl;
		throw std::runtime_error("Error: Failed to set socket option");
	}
}

void Socket::__init__SocketoptAuto(int opt=1) {
	setSocketOption(SOL_SOCKET, SO_REUSEADDR, opt); // 소켓 재사용 옵션 설정
	setSocketOption(SOL_SOCKET, SO_REUSEPORT, opt); // 포트 재사용 옵션 설정
	setSocketOption(SOL_SOCKET, SO_KEEPALIVE, opt); // keep-alive 옵션 설정 --> 연결이 끊어진 경우 자원을 반환하고 연결을 끊음 --> 리소스 절약
}

/* Socket 자동 활성화 */

void Socket::autoActivate(int domain, int type, int protocol) {
	socket(domain, type, protocol);
	std::cout << "Socket activated" << std::endl;
	__init__SocketoptAuto();
	std::cout << "Socket options set" << std::endl;
	nonblocking();
	std::cout << "Socket non-blocking set" << std::endl;
	bind(_host, _port);
	std::cout << "Socket bound" << std::endl;
	listen();
	std::cout << "Socket listening" << std::endl;
}

/* Socket 복제 */

Socket *Socket::clone() const {
	return new Socket(*this);
}

/* Socket 소멸자 */

Socket::~Socket() {
	
	if (_listenSocket == -1) {
		std::cout << "Socket already closed" << std::endl;
	} else {
		int status = ::close(_listenSocket);
		if (status == -1) {
			std::cerr << "Error: Socket close failed. Error code: " << errno << std::endl;
		} else {
			std::cout << "Socket closed" << std::endl;
		}
	}
}

/* Socket 할당 해제 */

Socket& Socket::operator=(const Socket& other) {
    if (this != &other) {
        if (::close(_listenSocket) == -1) {
            std::cerr << "Error: Failed to close socket before assignment. Error code: " << errno << std::endl;
        }
        this->_listenSocket = dup(other._listenSocket);
        if (this->_listenSocket == -1) {
            std::cerr << "Error: Failed to duplicate socket during assignment. Error code: " << errno << std::endl;
            throw std::runtime_error("Error: Failed to duplicate socket during assignment");
        }
        this->_server_Addr = other._server_Addr;
    }
    return *this;
}