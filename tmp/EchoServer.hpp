#ifndef ECHOSERVER_HPP
# define ECHOSERVER_HPP
# include <iostream>
# include <vector>
# include <map>
# include <cstdio>
# include <unistd.h>
# include <sys/time.h>
# include <sys/event.h>
# include <sys/types.h>
# include <algorithm>

# include "../socket/socket.hpp"
# include "../Parse/Config.hpp"
# include "../request/Client.hpp"
# include "../utils/utils.hpp"
# include "../request/Request.hpp"

# define READ 0
# define WRITE 1

class Server
{
	private:
		// std::vector<FD> _fdList;
		std::vector<struct kevent> _changeList;
		std::vector<Socket *> _socketList;
		std::map<FD, std::string> _clients;
		FD _kq;
	protected:
		void activateSocket(const Config &Conf);
		void readEventList(int eventNumber, struct kevent *eventList, const Config &Conf);
		void eventHandling(struct kevent &currEvent, const Config &Conf);
		void disconnectClient(int fd);
		void addNewClient(FD fd);
		// void execute(Client *ClientPtr, const Config &Conf);
		// void executeCGI(Client &param);
		int socketFDIndex(FD fd);
		void changeEvents(std::vector<struct kevent> &changeList, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);

	public:
		Server();
		~Server();

		void makeServerSocket(Config &Conf);
		void queueInit(const Config &Conf);
		void run(const Config &Conf);
};
#endif
