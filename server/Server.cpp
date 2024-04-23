// 다들 .hpp 파일에 작성중이니까 현재 .cpp 파일을 .hpp 파일에 나중에 합치겠음

#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

//Config에 있는 server block 의 갯수를 반환해서 해당 갯수만큼 소켓을 생성.
void Server::makeServerSocket(Config &Conf) {
	for (int i = 0; i < Conf.getNumberOfServer(); ++i) {
		Socket elem(Conf.getHostName(i), Conf.getPortNumber(i));
		_socketList.push_back(elem);
	}
}
//서버 소켓을 bind -> listen 상태로 만들어야 한다.
void Server::activateSocket(const Config &Conf) {
	for (int i = 0; i < _socketList.size(); ++i) {
		_socketList[i].autoActivate();
	}
}
//이벤트 큐를 받아오고, 이 이벤트 큐는 READ 이벤트만 받아온다.(동작에 따라 쓰기 이벤트를 나중에 등록.)
void Server::queueInit() {
	_kq = kqueue();
	for(int i = 0; i < _socketList.size(); ++i) {
		changeEvents(_changeList, static_cast<FD>(_socketList[i]), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	}
}

//새로운 이벤트를 넣을 때 쓰는 기능함수.
void Server::changeEvents(std::vector<struct kevent> &changeList, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent tempEvent;

    EV_SET(&tempEvent, ident, filter, flags, fflags, data, udata);
    changeList.push_back(tempEvent);
}

//클라이언트가 접속 종료를 했거나, 오류가 생겼을 때 해당 fd 를 닫아준다.
void Server::disconnectClient(int fd) {
    close(fd);
    delete _clients[fd];
	_clients[fd] = nullptr;
}

//서버를 시작한댜. while 문으로 무한 반복하면서, 큐의 앞쪽에 있는 이벤트 10개를 받아와서 우선적으로 멀티플렉싱 처리.
//_changeList 는 queue에 이벤트를 "추가" 하기 위한 것으로 kevent 가 호출되고 나면 초기화 해야 한다.
void Server::run(const Config &Conf) {
	int eventNumber;
	struct kevent eventList[10];

	while (true) {
		eventNumber = kevent(_kq, &_changeList[0], _changeList.size(), eventList, 100, NULL);
		if (eventNumber == -1)
			throw std::exception(); // 에러 처리 해야댐
		_changeList.clear();
		readEventList(eventNumber, eventList, Conf);
	}
}

//eventList 순회하는 동작.
void Server::readEventList(int eventNumber, struct kevent *eventList, const Config &Conf) {
	for (int i = 0; i < eventNumber; ++i) {
		eventHandling(eventList[i], Conf);
	}
}

//각 이벤트를 flag 를 읽어서 처리한다.
//에러 -> 에러
//읽기 -> 버퍼를 넣고 완성되었는지 플래그를 확인. 다 읽었으면 해당 fd 의 읽기 이벤트를 죽이고 쓰기 이벤트를 넣어서 WRITE 이벤트 감지를 한다.
	//각 소켓 읽기 동작이 끝났는지는 Client 클래스에서 판단해야 한다.
	//그렇기 때문에 버퍼를 받을 때마다 체크해서 헤더를 다 읽었으면, 헤더 파싱을,
	//바디를 받았으면 바디 길이가 content length 와 같을 때 COMPLETE 플래그를 띄워야 한다.
//쓰기 -> 클라이언트에게 완성된 HTTP 메세지를 보낸다. ERROR 플래그가 있다면, 해당 ERROR 에 맞게 에러 페이지를 띄워야 한다.
//PROC -> 자식 프로세스가 종료 되었을 때 처리해야 하는 부분이다.

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
			if (length = recv(currEvent.ident, buffer, 1024, 0) == -1)
				return ;
			else if (length == 0)
				disconnectClient(currEvent.ident);
			else {
				if (_clients[currEvent.ident] == nullptr)
					return ;
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

//서버 이벤트 큐를 읽는데, 읽기 이벤트가 발생했고, 이 이벤트가 서버 소켓이면, 새로운 연결 요청을 하는 것이다.
//accept 로 소켓을 뚫어서 새로운 fd 를 할당하고, 그 fd 와 맞는 Client class 을 만든다.
void Server::addNewClient(FD fd) {
	int newFD;
	Client *temp;
	
	temp = new Client();
	newFD = _socketList[socketFDIndex(fd)].accept();
	_clients.insert(std::pair<FD, Client *>{newFD, temp});
}

//Socket class 는 벡터로 관리되고 있기 때문에, fd 를 입력했을 때 해당 fd 를 가진 class 의 인덱스를 반환한다.
int Server::socketFDIndex(FD fd) {
	for (int i = 0; i < _socketList.size(); ++i) {
		if (static_cast<FD>(_socketList[i]) == fd)
			return i ;
	}
	return -1 ;
}
//작성중, flag의 에러 여부를 살피고, method 를 판단해서 각 메소드마다 다르게 동작하는 if 문을 작성한다. 
void Server::execute(Client *ClientPtr, const Config &Conf) {
	if (ClientPtr->flag == ERROR)
		responseError(flag);
	if (ClientPtr->getMethod() == "GET") {
		ClientPtr->get
	}
}

void Server::executeCGI(Client &param) {}
