#include "EchoServer.hpp"

Server::Server() {}

Server::~Server() {}

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

void Server::disconnectClient(int fd) {
    close(fd);
	delete _clientMap[fd];
	_clientMap[fd] = NULL;
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
	newFD = accept(fd, NULL, NULL);
	// newFD = _serverSocketList[FDIndexing(fd)]->accept();
	std::cout << "hi new client. | fd : " << newFD << std::endl;
	Socket::nonblocking(newFD);
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


void Server::handleClientRead(FD clientFd, const Config &Conf) {
    unsigned char *buffer = new unsigned char[1025];
    ssize_t length;

    length = recv(clientFd, buffer, 1024, 0);
    buffer[length] = '\0';
    
    if (length < 0) {
        // 오류 발생 시 클라이언트 연결 종료
        std::cerr << "Error reading from client: " << clientFd << std::endl;
        delayResponse(0.0002f);
        delete[] buffer;
        // disconnectClient(clientFd);
        return ;
    } else if (length == 0) {
        // 클라이언트가 연결을 종료한 경우
        std::cout << "Client disconnected: " << clientFd << std::endl;
        disconnectClient(clientFd);
        delete[] buffer;
        return ;
    }
    
    Client *ptr = _clientMap[clientFd];
    ptr->setBuffer(buffer, length);
    delete[] buffer;
    
    if (ptr->getReadStatus() == READ_DONE || ptr->getReadStatus() == READ_ERROR) {
        // std::cout << "Read Done" << std::endl;

        if (ptr->getResponseHandle().isCGI()) {
            ptr->getProcInfo()->ClientFd = clientFd;
            changeEvents(_changeList, ptr->getProcInfo()->pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT | NOTE_EXITSTATUS, 0, ptr->getProcInfo());
            changeEvents(_changeList, clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
        } else {
            ptr->generateResponse(Conf);
            changeEvents(_changeList, clientFd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
            changeEvents(_changeList, clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
        }
        return ;
    }
}

//main에서 기본 세팅이 끝나면, 이걸 실행해야 한다.
void Server::run(const Config &Conf) {
	int eventNumber;
	struct kevent eventList[100];
	while (true) {
		eventNumber = kevent(_kq, &_changeList[0], _changeList.size(), eventList, 100, NULL);
		// std::cerr << "---------Current event--------- num | " << eventNumber << std::endl;
		updateControl();
		if (eventNumber == -1)
			throw std::runtime_error("asdf");
		for (int i = 0; i < eventNumber; ++i) {
			std::cout << "Event occurred: " << eventList[i].ident << ": " << eventList[i].filter << std::endl;
			eventHandling(eventList[i], Conf);
		}
	}
}

// 각각 이벤트가 가지고 있는 ident(fd) 값과 flag 값을 확인해서 해당 이벤트를 파악 후 거기에 맞게 실행한다.
// 나중에 리펙토링 할 예정.
void Server::eventHandling(struct kevent &currEvent, const Config &Conf) {
	std::vector<char> buffer(1024);
	ssize_t length;
	
	Conf.getNumberOfServer(); // -Wall -Extra -Error 플래그때문에 그냥 넣어놨음---------------
	if (currEvent.flags & EV_ERROR) {
		return ;
	}
	if (currEvent.filter == EVFILT_TIMER)
		disconnectClient(currEvent.ident);
	else if (currEvent.filter == EVFILT_READ) {
		if (FDIndexing(currEvent.ident) >= 0) {
			addNewClient(currEvent.ident);
			return ;
		}
		length = read(currEvent.ident, buffer.data(), buffer.size());
		if (length < 0)
			return ; 
		if (length == 0) {
			std::cout << "Disconnect Client: " << currEvent.ident << std::endl;
			disconnectClient(currEvent.ident);
		} else {
			Client *ptr = _clientMap[currEvent.ident];
			ptr->setBuffer(buffer.data());
			if (ptr->getReadStatus() == READ_DONE || ptr->getReadStatus() == READ_ERROR) {
				if (ptr->getResponseHandle().isCGI() == true) {
					ptr->getProcInfo()->ClientFd = currEvent.ident;
					changeEvents(_changeList, ptr->getProcInfo()->pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT | NOTE_EXITSTATUS, 0, ptr->getProcInfo());
					changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
				} else {
					ptr->generateResponse(Conf);
					changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
					changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
				}
			}
		}
	} else if (currEvent.filter == EVFILT_WRITE) {
		Client *ptr = _clientMap[currEvent.ident];
		length = send(currEvent.ident, ptr->getResponse().c_str(), ptr->getResponse().length(), 0);
		if (length <= 0) {
			disconnectClient(currEvent.ident);
		} else {
			ptr->cutResponse(length);
			if (ptr->getResponse().length() == 0) {
				if (ptr->getIsKeepAlive() == false) {
					std::cout << "=============================================== close" << std::endl;
					disconnectClient(currEvent.ident);
				} else {
					ptr->clearAll();
					changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
					changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
					changeEvents(_changeList, currEvent.ident, EVFILT_TIMER, EV_DISABLE, 0, 0, NULL);
					changeEvents(_changeList, currEvent.ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, setConnectTime(5), NULL);
				}
			}
		}
	}
	// else if (currEvent.filter == EVFILT_PROC) {
	// 	procInfo *procPtr = static_cast<procInfo *>(currEvent.udata);
	// 	Client *ptr = _clientMap[procPtr->ClientFd];
	// 	ptr->setBufferFromChild(currEvent.data);
	// 	// changeEvents(_changeList, )
	// }
}
