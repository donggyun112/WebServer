// 다들 .hpp 파일에 작성중이니까 현재 .cpp 파일을 .hpp 파일에 나중에 합치겠음

#include "EchoServer.hpp"

Server::Server() {}

Server::~Server() {}

//Config에 있는 server block 의 갯수를 반환해서 해당 갯수만큼 소켓을 생성.
void Server::makeServerSocket(/*Config &Conf*/) {
	for (int i = 0; i < 1; ++i) {
		Socket *elem = new Socket("127.0.0.1", 8080);
		
		std::cout << "socket : " << elem->getHost() << "port : " << elem->getPort() << std::endl;
		//Socket elem(Conf.getHostName(i), Conf.getPortNumber(i));
		_socketList.push_back(elem);
		activateSocket();
	}
}

//서버 소켓을 bind -> listen 상태로 만들어야 한다.
void Server::activateSocket(/*const Config &Conf*/) {
	_socketList[0]->autoActivate();
}

void Server::queueInit() {
	_kq = kqueue();
	std::cout << "---------------kq" << _kq << std::endl << std::endl;
	for(int i = 0; i < 1; ++i) {
		changeEvents(_changeList, static_cast<FD>(*_socketList[i]), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	}
}

void Server::changeEvents(std::vector<struct kevent> &changeList, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent tempEvent;

    EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
    changeList.push_back(tempEvent);
}


void Server::disconnectClient(int fd) {
    close(fd);
	_clients[fd] = "";
	// _clients[fd] = nullptr;
}

void Server::run(/*const Config &Conf*/) {
	int eventNumber;
	struct kevent eventList[10];
	while (true) {
		eventNumber = kevent(_kq, &_changeList[0], _changeList.size(), eventList, 100, NULL);
		std::cout << "event-------------- " << eventNumber << std::endl;
		if (eventNumber == -1)
			throw std::runtime_error("asdf"); // 에러 처리 해야댐
	std::cout << __FILE__ << " ; " << __LINE__ << std::endl;
		_changeList.clear();
		readEventList(eventNumber, eventList/*, Conf*/);
	}
}

//Socket class 는 벡터로 관리되고 있기 때문에, fd 를 입력했을 때 해당 fd 를 가진 class 의 인덱스를 반환한다.
int Server::socketFDIndex(FD fd) {
	for (int i = 0; i < _socketList.size(); ++i) {
		if (static_cast<FD>(*_socketList[i]) == fd)
			return i ;
	}
	return -1 ;
}

void Server::readEventList(int eventNumber, struct kevent *eventList/*, const Config &Conf*/) {
	for (int i = 0; i < eventNumber; ++i) {
		eventHandling(eventList[i]/*, Conf*/);
	}
}

void Server::addNewClient(FD fd) {
	int newFD;
	std::cout << "server socket : " << fd << std::endl;
std::cout << "hi new client" << std::endl;
	newFD = _socketList[socketFDIndex(fd)]->accept();
std::cout << "New Client : " << "fd = " << newFD << std::endl;
	Socket::nonblocking(newFD);
	_clients.insert(std::pair<FD, std::string>(newFD, ""));
	changeEvents(_changeList, newFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(_changeList, newFD, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
}

void Server::eventHandling(struct kevent &currEvent/*, const Config &Conf*/) {
	char buffer[1024];
	ssize_t length;

	std::memset(buffer, 0, sizeof(buffer));
	if (currEvent.flags & EV_ERROR) {
		if (socketFDIndex(currEvent.ident) >= 0) {
			throw std::runtime_error("server_socket_error"); // 서버 소켓 에러.
			//이 부분은 그냥 터트리는게 맞다 ㄹㅇ;;;
		}
		else {
			disconnectClient(currEvent.ident);
			throw std::runtime_error("client_socket_error"); // 클라이언트 소켓 에러.
		}
	}
	else if (currEvent.filter == EVFILT_READ) {
		if (socketFDIndex(currEvent.ident) >= 0) {
			addNewClient(currEvent.ident);
			//test
			return ;
		}
		else {
			length = recv(currEvent.ident, buffer, 1024, 0);
			std::cout << "buffer = " << buffer << "| length : " << length << std::endl;
			if (length == -1)
				return ;
			else if (length == 0) {
				close(currEvent.ident);
				std::cout << "bye bye" << std::endl;
				changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
				changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				std::cout << "Client closed | fd : " << currEvent.ident << std::endl;
			}
			else {
				std::string temp(buffer, length);
				std::cout << "READ | fd : " << currEvent.ident << "buffer = " << buffer << std::endl;
				_clients[currEvent.ident] += temp;
				changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
			 	changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
		 		std::memset(buffer, 0, sizeof(buffer));
			}
		}
	}
	else if (currEvent.filter == EVFILT_WRITE) {
		std::cout << "ECHO | fd : " << currEvent.ident << "| data : " << _clients[currEvent.ident].c_str() << "|" << std::endl;
		send(currEvent.ident, _clients[currEvent.ident].c_str(), _clients[currEvent.ident].length(), 0);
		_clients[currEvent.ident] = "";
		changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);	
		changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
	}
}
