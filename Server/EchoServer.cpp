#include "EchoServer.hpp"
#include <sys/time.h>

Server::Server() {}

Server::~Server() {
	// 동적으로 할당한 메모리 해제
	for (std::vector<Socket *>::iterator it = _serverSocketList.begin(); it != _serverSocketList.end(); ++it) {
		delete *it;
	}
	for (std::map<FD, Client *>::iterator it = _clientMap.begin(); it != _clientMap.end(); ++it) {
		delete it->second;
	}
}

//Config에 있는 server block 의 갯수를 반환해서 해당 갯수만큼 소켓을 생성.
void Server::makeServerSocket(Config &Conf) {
	for (int i = 0; i < Conf.getNumberOfServer(); i++) {
		Socket *elem = new Socket(Conf[i].getServerName(), Conf[i].getPortName());
		_serverSocketList.push_back(elem);
	}
	activateSocket(Conf);
}

//서버 소켓을 bind -> listen 상태로 만들어야 한다.
void Server::activateSocket(const Config &Conf) {
	for (int i = 0; i < Conf.getNumberOfServer(); ++i)
		_serverSocketList[i]->autoActivate();
}

int Server::nonblocking() {
    int flags = fcntl(_kq, F_GETFL, 0);

    if (flags == -1) {
        std::cerr << "Error: Failed to get socket flags. Error code: " << errno << std::endl;
        return FAILURE;
    }

    flags |= O_NONBLOCK;
    if (fcntl(_kq, F_SETFL, flags) == -1) {
        std::cerr << "Error: Failed to set socket flags. Error code: " << errno << std::endl;
        return FAILURE;
    }
    return SUCCESS;
}

void Server::queueInit(const Config &Conf) {
	_kq = kqueue();

	for(int i = 0; i < Conf.getNumberOfServer(); ++i) {
		changeEvents(_changeList, static_cast<FD>(*_serverSocketList[i]), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	}
}

void Server::changeEvents(std::vector<struct kevent> &changeList, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent tempEvent;

    EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
    changeList.push_back(tempEvent);
}

time_t Server::setConnectTime(int sec) {
	struct timespec timeout;

	timeout.tv_sec = sec; // 60초 타임아웃 설정
	timeout.tv_nsec = 0;	
	return timeout.tv_sec;
}

//Socket class 는 벡터로 관리되고 있기 때문에, fd 를 입력했을 때 해당 fd 를 가진 class 의 인덱스를 반환하는 함수를 만들었음.
int Server::FDIndexing(FD fd) {
	for (size_t i = 0; i < _serverSocketList.size(); ++i) {
		if (static_cast<FD>(*_serverSocketList[i]) == fd)
			return i;
	}
	return -1;
}

//새로운 client를 만드는 프로세스이다. accept로 새로운 fd 할당, non-blocking 설정, client 클래스추가, 해당 fd 에 대한 읽기 쓰기 이벤트 만들기가 순차적으로 진행된다.
void Server::addNewClient(FD fd) {
	int newFD;
	int opt = 1;
	
	newFD = Socket::accept(fd);
	if (newFD == -1) {
		std::cerr << "Error accepting new client" << std::endl;
		return;
	}


	std::cout << "hi new client. | fd : " << newFD << std::endl;
	setsockopt(newFD, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
	setsockopt(newFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	setsockopt(newFD, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
	setsockopt(newFD, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
	// Socket::nonblocking(newFD);
	Client *Ptr = new Client(_serverSocketList[FDIndexing(fd)]->getPort());
	_clientMap[newFD] = Ptr;
	changeEvents(_changeList, newFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(_changeList, newFD, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
	// changeEvents(_changeList, newFD, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, setConnectTime(5), NULL);
}

//kevent 를 통해서 이벤트를 죽이고 나서 해당 fd소켓을 닫아야 에러 이벤트가 발생하지 않는다. 그래서 kevent 를 실행하고 나서 해야하는 update를 한번에 관리하는 함수를 추가.
void Server::updateControl() {
	_changeList.clear();
}


void Server::disconnectClient(int fd) {
    
    // 클라이언트 맵에서 해당 fd의 클라이언트 객체 찾기
    std::map<FD, Client *>::iterator it = _clientMap.find(fd);
    if (it == _clientMap.end()) {
        std::cout << "Client not found: " << fd << std::endl;
        return;
    }
    
    delete it->second;
    _clientMap.erase(it);
    
    changeEvents(_changeList, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    changeEvents(_changeList, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    std::cout << "Client disconnected: " << fd << std::endl;
	// usleep(1000);

	if (close(fd) == -1) {
        std::cerr << "Error closing socket: " << fd << std::endl;
    }
	delayResponse(0.0002f);

}



void Server::delayResponse(double seconds) {
    clock_t startTime = clock();
    double elapsedSeconds = 0.0;

    while (elapsedSeconds < seconds) {
        clock_t currentTime = clock();
        elapsedSeconds = static_cast<double>(currentTime - startTime) / CLOCKS_PER_SEC;
    }
}


void Server::handleClientRead(FD clientFd, const Config &Conf, char **env) {
    std::vector<char> buffer(1024);
    ssize_t length;
    
    length = recv(clientFd, buffer.data(), buffer.size(), 0);
    
    if (length < 0) {
        // 오류 발생 시 클라이언트 연결 종료
        std::cerr << "Error reading from client: " << clientFd << std::endl;
        delayResponse(0.0002f);
        // disconnectClient(clientFd);
        // 잠시 대기
        return;
    } else if (length == 0) {
        // 클라이언트가 연결을 종료한 경우
        std::cout << "Client disconnected: " << clientFd << std::endl;
        disconnectClient(clientFd);
        return;
    }
    
    Client *ptr = _clientMap[clientFd];
    ptr->setBuffer(buffer.data());
    
    if (ptr->getReadStatus() == READ_DONE || ptr->getReadStatus() == READ_ERROR) {
        std::cout << "Read Done" << std::endl;
        ptr->generateResponse(Conf, env);
        if (ptr->getResponseHandle().isCGI()) {
            std::cout << "iscgi == true, start to make proc event" << std::endl;
            ptr->getProcInfo()->clientFd = clientFd;
            changeEvents(_changeList, ptr->getProcInfo()->pid, EVFILT_PROC, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_EXIT | NOTE_EXITSTATUS, 0, ptr->getProcInfo());
            changeEvents(_changeList, clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
        } else {
            changeEvents(_changeList, clientFd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
            changeEvents(_changeList, clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
        }
        return ;
    }
}

void Server::handleClientCgi(struct kevent &currEvent, const Config & Conf) {
	procInfo *procPtr = static_cast<procInfo *>(currEvent.udata);
	Client *ptr = _clientMap[procPtr->clientFd];
	int tempFileFd;
	int length = -1;
	char buffer[1024];

	if (currEvent.data == InternalServerError_500) {
		std::remove(procPtr->tempFilePath.c_str());
		ptr->setResponse(Error::errorHandler(Conf[ptr->getPort()], InternalServerError_500));
		waitpid(procPtr->pid, NULL, 0);
		changeEvents(_changeList, procPtr->clientFd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
		changeEvents(_changeList, procPtr->clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
		return ;
	}
	tempFileFd = open(procPtr->tempFilePath.c_str(), O_RDONLY);
	while (length) {
		length = read(tempFileFd, buffer, 1024);
		if (length == -1)
			break;
		ptr->appendResponse(buffer);
	}
	close(tempFileFd);
	// std::remove(procPtr->tempFilePath.c_str());
	waitpid(procPtr->pid, NULL, 0);
	if (length == -1) {
        std::cerr << "length == -1" << std::endl;
		// ptr->setResponse(Error::errorHandler(Conf[ptr->getPort()], InternalServerError_500));
		changeEvents(_changeList, procPtr->clientFd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
		changeEvents(_changeList, procPtr->clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
	} else {
		changeEvents(_changeList, procPtr->clientFd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
	}
}


//main에서 기본 세팅이 끝나면, 이걸 실행해야 한다.
void Server::run(const Config &Conf, char **env) {
    int eventNumber;
    struct kevent eventList[1024];
    
    struct timespec timeout;
    timeout.tv_sec = 10; // 1초
    timeout.tv_nsec = 0;
    
    while (true)
	{
		// std::cout << "---------Server is running---------" << std::endl;
		eventNumber = kevent(_kq, &_changeList[0], _changeList.size(), eventList, 10, NULL);
		// std::cout << "---------Server is running---------" << std::endl;
        std::cout << "---------Current event--------- num | " << eventNumber << std::endl;
        
        updateControl();
        
        if (eventNumber < 0)
            continue;
        
        for (int i = 0; i < eventNumber; ++i) {
            std::cout << "Event occurred: " << eventList[i].ident << ": " << eventList[i].filter << std::endl;
            
            if (eventList[i].flags & EV_ERROR) {
                std::cout << "Event error occurred" << std::endl;
				// disconnectClient(eventList[i].ident);
                break;
            }
            
            if (eventList[i].filter == EVFILT_TIMER) {
                disconnectClient(eventList[i].ident);
            } else if (eventList[i].filter == EVFILT_READ) {
				
                if (FDIndexing(eventList[i].ident) >= 0) {
                    addNewClient(eventList[i].ident);
                } else {
                    handleClientRead(eventList[i].ident, Conf, env);
                }
            } else if (eventList[i].filter == EVFILT_WRITE) {
				const float fixedDelaySeconds = 0.0018f;
				const clock_t fixedDelayClocks = static_cast<clock_t>(fixedDelaySeconds * CLOCKS_PER_SEC);

				clock_t startTime = clock();
				handleClientWrite(eventList[i].ident, Conf);
				clock_t endTime = clock();

				clock_t elapsedClocks = endTime - startTime;
				if (elapsedClocks < fixedDelayClocks) {
					// std::cout << "-----------------delay response-----------------" << std::endl;
					float remainingSeconds = std::round(static_cast<float>(fixedDelayClocks - elapsedClocks) / CLOCKS_PER_SEC * 100000.0f) / 100000.0f;
					std::cout << "remainingSeconds : " << remainingSeconds << std::endl;
					delayResponse(remainingSeconds);
				}
			} else if (eventList[i].filter == EVFILT_PROC) {
				handleClientCgi(eventList[i], Conf);
			}
        }
    }
}

void Server::handleClientWrite(FD clientFd, const Config &Conf) {
    Client *ptr = _clientMap[clientFd];
	(void)Conf;

	// std::cout << "HandleClientWrite | str = " << ptr->getResponse() << std::endl;
    ssize_t length = send(clientFd, ptr->getResponse().c_str(), ptr->getResponse().length(), 0);
	// usleep(1000);

    
    if (length <= 0) {
		// throw std::runtime_error("Error: Failed to send response to client.");

        disconnectClient(clientFd);
        return;
    }
    
    ptr->cutResponse(length);
    
    if (ptr->getResponse().length() == 0) {
        if (!ptr->getIsKeepAlive()) {
            // std::cout << "=============================================== close" << std::endl;
            disconnectClient(clientFd);
        } else {
            ptr->clearAll();
            changeEvents(_changeList, clientFd, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
            changeEvents(_changeList, clientFd, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
        }
    }
}