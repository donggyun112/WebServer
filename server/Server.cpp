#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

void Server::makeServerSocket(Config &Conf) {
	Socket *elem;
	for (int i = 0; i < Conf.getNumberOfServer(); ++i) {
		elem = new Socket(Conf.getHostName(i), Conf.getPortNumber(i));
		_socketList.push_back(elem);
		elem = nullptr;
	}
}

void Server::activateSocket(const Config &Conf) {
	for (int i = 0; i < _socketList.size(); ++i) {
		/*동균이오면 체크*/
	}
}

void Server::queueInit() {
	_kq = kqueue();
	for(int i = 0; i < _socketList.size(); ++i) {
		changeEvents(_changeList, static_cast<FD>(*_socketList[i]), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	}
}

void Server::changeEvents(std::vector<struct kevent> &changeList, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    changeList.push_back(temp_event);
}

void Server::disconnectClient(int fd) {
    close(fd);
    _clients.erase(fd);
}

void Server::run(const Config &Conf) {
	int eventNumber;
	struct kevent eventList[100];

	while (true) {
		eventNumber = kevent(_kq, &_changeList[0], _changeList.size(), eventList, 100, NULL);
		if (eventNumber == -1)
			throw std::exception(); // 에러 처리 해야댐
		_changeList.clear();
		readEventList(eventNumber, eventList, Conf);
	}
}

void Server::readEventList(int eventNumber, struct kevent *eventList, const Config &Conf) {
	for (int i = 0; i < eventNumber; ++i) {
		eventHandling(eventList[i], Conf);
	}
}

void Server::eventHandling(struct kevent &currEvent, const Config &Conf) {
	char buffer[1024];
	ssize_t length;

	if (currEvent.flags & EV_ERROR) {
		if (socketFDIndex(currEvent.ident) >= 0) {
			throw std::exception(); // 서버 소켓 에러.
			//이 부분은 그냥 터트리는게 맞다 ㄹㅇ;;;
		}
		else {
			disconnectClient(currEvent.ident);
			throw std::exception(); // 클라이언트 소켓 에러.
		}
	}
	else if (currEvent.filter = EVFILT_READ) {
		if (socketFDIndex(currEvent.ident) >= 0) {
			addNewClient(currEvent.ident);
			return ;
		}
		else {
			if (length = recv(currEvent.ident, buffer, 1024, 0)== -1)
				return ;
			else if (length == 0)
				disconnectClient(currEvent.ident);
			else {
				_clients[currEvent.ident]->clientRecv(buffer);
				if (_clients[currEvent.ident]->getCompleteFlag() == COMPLETE) {
					execute(_clients[currEvent.ident], Conf);
					changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				}
			}
		}
	}
}

void Server::addNewClient(FD fd) {
	int newFD;
	Client *temp;
	
	temp = new Client();
	newFD = _socketList[socketFDIndex(fd)]->accept();
	_clients.insert(std::pair<FD, Client *>{newFD, temp});
}

int Server::socketFDIndex(FD fd) {
	for (int i = 0; i < _socketList.size(); ++i) {
		if (static_cast<FD>(*_socketList[i]) == fd)
			return i ;
	}
	return -1 ;
}

void Server::execute(Client *paramPtr, const Config &Conf) {}

void Server::executeCGI(Client &param) {}
