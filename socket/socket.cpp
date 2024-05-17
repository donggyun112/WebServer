#include "socket.hpp"

/* Socket 기본설정 */

void Socket::__init__(std::string host, Port port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);

    int status = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &_addrInfo);
    if (status != 0) {
        std::cerr << "Error: Failed to get address info. Error code: " << status << std::endl;
        throw std::runtime_error("Error: Failed to get address info");
    }
}

int Socket::getSocketOPT(const FD &socket) {
	int flags;
	socklen_t optlen = sizeof(flags);

	if (::getsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &flags, &optlen) == FAILURE) {
		std::cerr << "Error: Failed to get socket option. Error code: " << errno << std::endl;
		throw std::runtime_error("Error: Failed to get socket option");
	}
	return flags;
}

void Socket::__init__(Port port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(port);

    int status = getaddrinfo(NULL, port_str.c_str(), &hints, &_addrInfo);
    if (status != 0) {
        std::cerr << "Error: Failed to get address info. Error code: " << status << std::endl;
        throw std::runtime_error("Error: Failed to get address info");
    }
}

// Socket 클래스 생성자

Socket::Socket(std::string host, Port port) : _listenSocket(-1), _port(port), _host(host) {
    if (host.empty()) {
        std::cout << "Host is empty" << std::endl;
        __init__(port);
    } else {
        host = "localhost";
        __init__(host, port);
    }
    std::cout << "Socket initialized with host and port" << std::endl;
}

Socket::Socket(const Socket& other) : _port(other._port), _host(other._host), _addrInfo(other._addrInfo) {
    if (other._listenSocket != -1) {
        _listenSocket = dup(other._listenSocket);
    }
    std::cout << "Socket copied" << std::endl;
}

/* Socket 생성 */
FD Socket::socket(int domain, int type, int protocol) {
    std::cout << "Socket created" << std::endl;
    _listenSocket = ::socket(domain, type, protocol);
    if (_listenSocket == -1) {
        std::cerr << "Error: Socket creation failed. Error code: " << errno << std::endl;
        throw std::runtime_error("Error: Socket creation failed");
    }
	try {
		getProtocolName();
	} catch (std::runtime_error &e) {
		std::cerr << "Error: is not TCP protocol" << std::endl;
		throw std::runtime_error("Error: is not TCP protocol");
	}
    return _listenSocket;
}

/* Socket 바인딩 */
Status Socket::bind() {
    if (::bind(_listenSocket, _addrInfo->ai_addr, _addrInfo->ai_addrlen) == FAILURE) {
        std::cerr << "Error: Failed to bind socket. Error code: " << errno << std::endl;
        throw std::runtime_error("Error: Failed to bind socket");
    }
    return SUCCESS;
}

/* Socket 리스닝 */
Status Socket::listen(size_t backlog) {
    if (::listen(_listenSocket, backlog) == FAILURE) {
        std::cerr << "Error: Failed to listen socket. Error code: " << errno << std::endl;
        throw std::runtime_error("Error: Failed to listen socket");
    }
    return SUCCESS;
}

FD Socket::accept() const {
    FD Client_Socket = -1;

    Client_Socket = ::accept(_listenSocket, NULL, NULL);
    if (Client_Socket == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 클라이언트 연결 요청이 없는 경우, 다른 작업을 수행하거나 잠시 대기할 수 있습니다.
            // return Client_Socket;
            throw std::runtime_error("Error: Failed to accept socket");
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
    _listenSocket = -1;
    return SUCCESS;
}

/* Socket 비동기 활성화 */
Status Socket::nonblocking() {
    // int flags = Socket::getSocketOPT(_listenSocket);
    // if (flags == -1) {
    //     std::cerr << "Error: Failed to get socket flags. Error code: " << errno << std::endl;
    //     return FAILURE;
    // }
    int flags = fcntl(_listenSocket, F_GETFL, 0);

    flags |= O_NONBLOCK;
    flags |= O_CLOEXEC;
    if (fcntl(_listenSocket, F_SETFL, flags) == -1) {
        std::cerr << "Error: Failed to set socket flags. Error code: " << errno << std::endl;
        return FAILURE;
    }
    return SUCCESS;
}

Status Socket::nonblocking(const FD &socket) {
    // int flags = Socket::getSocketOPT(socket);
    // if (flags == -1) {
    //     std::cerr << "Error: Failed to get socket flags. Error code: " << errno << std::endl;
    //     return FAILURE;
    // }
    int flags = fcntl(socket, F_GETFL, 0);

    flags |= O_NONBLOCK;
    flags |= O_CLOEXEC;
    if (fcntl(socket, F_SETFL, flags) == -1) {
        std::cerr << "Error: Failed to set socket flags. Error code: " << errno << std::endl;
        return FAILURE;
    }
    return SUCCESS;
}

/* Socket 옵션 설정 */
void Socket::setSocketOption(int level, int option_name, int opt) {
    socklen_t optlen = sizeof(opt);

    if (setsockopt(this->_listenSocket, level, option_name, &opt, optlen) == FAILURE) {
        std::cerr << "Error: Failed to set socket option. Error code: " << errno << std::endl;
        throw std::runtime_error("Error: Failed to set socket option");
    }
}

void Socket::__init__SocketoptAuto(int opt) {
    (void)opt;
    setSocketOption(SOL_SOCKET, SO_REUSEADDR, opt);
    setSocketOption(SOL_SOCKET, SO_REUSEPORT, opt);
    setSocketOption(SOL_SOCKET, SO_KEEPALIVE, opt);
}

/* Socket 자동 활성화 */
void Socket::autoActivate(int domain, int type, int protocol) {
    socket(domain, type, protocol);
    std::cout << "Socket activated" << std::endl;
    __init__SocketoptAuto(1);
    std::cout << "Socket options set" << std::endl;
    nonblocking();
    std::cout << "Socket non-blocking set" << std::endl;
    bind();
    std::cout << "Socket bound" << std::endl;
    listen(1000);
    std::cout << "Socket listening" << std::endl;
	std::cout << "Host: " << _host << "Host IP: " << getSocketIP() << "Port: " << getPort() << "Protocol: " << getProtocolName() << std::endl;
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

    if (_addrInfo != NULL) {
        freeaddrinfo(_addrInfo);
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
        this->_addrInfo = other._addrInfo;
    }
    return *this;
}

/* 주소 정보 설정 */
void Socket::setAddrInfo(struct addrinfo *addrInfo) {
    _addrInfo = addrInfo;
}

std::string Socket::getSocketIP() const {
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((struct sockaddr_in *)_addrInfo->ai_addr)->sin_addr, ipstr, INET_ADDRSTRLEN);
    return std::string(ipstr);
}

Port Socket::getSocketPort() const {
    return ntohs(((struct sockaddr_in *)_addrInfo->ai_addr)->sin_port);
}

std::string Socket::getProtocolName() const {
    struct protoent *proto = getprotobyname("tcp");
    if (proto == NULL) {
        std::cerr << "Error: Failed to get protocol name. Error code: " << errno << std::endl;
        throw std::runtime_error("Error: Failed to get protocol name");
    }
    return std::string(proto->p_name);
}