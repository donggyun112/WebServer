#include "EchoServer.hpp"

Server::Server() {}

Server::~Server() {}

//Config에 있는 server block 의 갯수를 반환해서 해당 갯수만큼 소켓을 생성.
void Server::makeServerSocket(Config &Conf) {
	for (int i = 0; i < Conf.getNumberOfServer(); i++) {
		Socket *elem = new Socket(Conf[i].getServerName(), Conf[i].getPortName());
		_socketList.push_back(elem);
	}
	activateSocket(Conf);
}

//서버 소켓을 bind -> listen 상태로 만들어야 한다.
void Server::activateSocket(const Config &Conf) {
	for (int i = 0; i < Conf.getNumberOfServer(); ++i)
		_socketList[i]->autoActivate();
}

void Server::queueInit(const Config &Conf) {
	_kq = kqueue();
	for(int i = 0; i < Conf.getNumberOfServer(); ++i) {
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
}

//Socket class 는 벡터로 관리되고 있기 때문에, fd 를 입력했을 때 해당 fd 를 가진 class 의 인덱스를 반환하는 함수를 만들었음.
int Server::socketFDIndex(FD fd) {
	for (size_t i = 0; i < _socketList.size(); ++i) {
		if (static_cast<FD>(*_socketList[i]) == fd)
			return i;
	}
	return -1;
}

//새로운 client를 만드는 프로세스이다. accept로 새로운 fd 할당, non-blocking 설정, client 클래스추가, 해당 fd 에 대한 읽기 쓰기 이벤트 만들기가 순차적으로 진행된다.
void Server::addNewClient(FD fd) {
	int newFD;
	newFD = _socketList[socketFDIndex(fd)]->accept();
	std::cout << "hi new client. | fd : " << newFD << std::endl << std::endl;
	Socket::nonblocking(newFD);
	_clients.insert(std::pair<FD, std::string>(newFD, ""));
	changeEvents(_changeList, newFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(_changeList, newFD, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
}

//kevent 를 통해서 이벤트를 죽이고 나서 해당 fd소켓을 닫아야 에러 이벤트가 발생하지 않는다. 그래서 kevent 를 실행하고 나서 해야하는 update를 한번에 관리하는 함수를 추가.
void Server::updateControl() {
	for (size_t i = 0; i < _closeList.size(); i++) {
		disconnectClient(_closeList[i]);
		std::cout << "Client closed | fd : " << _closeList[i] << std::endl;
	}
	_closeList.clear();
	_changeList.clear();
}

//main에서 기본 세팅이 끝나면, 이걸 실행해야 한다.
void Server::run(const Config &Conf) {
	int eventNumber;
	struct kevent eventList[10];
	while (true) {
		eventNumber = kevent(_kq, &_changeList[0], _changeList.size(), eventList, 10, NULL);
		std::cout << "---------Current event----------- event_number | " << eventNumber << std::endl;
		if (eventNumber == -1)
			throw std::runtime_error("asdf"); // 에러 처리 해야댐
		updateControl();
		for (int i = 0; i < eventNumber; ++i) {
			eventHandling(eventList[i], Conf);
		}
	}
}

//각각 이벤트가 가지고 있는 ident(fd) 값과 flag 값을 확인해서 해당 이벤트를 파악 후 거기에 맞게 실행한다. 나중에 리펙토링 할 예정.
void Server::eventHandling(struct kevent &currEvent, const Config &Conf) {
	char buffer[1024];
	ssize_t length;
	Conf.getNumberOfServer(); // 플래그때문에 그냥 넣어놨음---------------

	std::memset(buffer, 0, sizeof(buffer));
	if (currEvent.flags & EV_ERROR) {
		if (socketFDIndex(currEvent.ident) >= 0) {
			throw std::runtime_error("server_socket_error"); // 서버 소켓 에러.
			//이 부분은 그냥 터트리는게 맞다 ㄹㅇ;;;
		}
		else {
			std::cout << "Err | filter: " << currEvent.filter << " | flag: " << currEvent.flags << " fflags: " << currEvent.fflags << std::endl;
			_closeList.push_back(currEvent.ident);
			changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
			changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		}
	}
	else if (currEvent.filter == EVFILT_READ) {
		if (socketFDIndex(currEvent.ident) >= 0) {
			addNewClient(currEvent.ident);
			return ;
		}
		else {
			length = recv(currEvent.ident, buffer, 1024, 0);
			if (length == -1)
				return ;
			else if (length == 0) {
				std::cout << "Bye bye client. | fd : "<< currEvent.ident << std::endl;
				changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_DISABLE | EV_DELETE, 0, 0, NULL);
				changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_DISABLE | EV_DELETE, 0, 0, NULL);
				_closeList.push_back(currEvent.ident);
			}
			else {
				std::string temp(buffer, length);
				std::cout << "READ | fd : " << currEvent.ident << "| buffer = " << buffer << std::endl;
				_clients[currEvent.ident] += temp;
				changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
			 	changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
		 		std::memset(buffer, 0, sizeof(buffer));
			}
		}
	}
	else if (currEvent.filter == EVFILT_WRITE) {
		std::cout << "ECHO | fd : " << currEvent.ident << "| buffer = " << _clients[currEvent.ident].c_str() << std::endl;
		send(currEvent.ident, _clients[currEvent.ident].c_str(), _clients[currEvent.ident].length(), 0);
		_clients[currEvent.ident] = "";
		changeEvents(_changeList, currEvent.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);	
		changeEvents(_changeList, currEvent.ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
	}
}
