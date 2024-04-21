#ifndef SERVER_HPP
# define SERVER_HPP
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
# include "../Structure/config.hpp"

# define READ 0
# define WRITE 1

class Server 
{
	private:
		std::vector<FD> _serverList;
		std::map<FD, class Client> _clientList;
		std::vector<struct kevent> _changeList;
		std::vector<Socket *> _socketList;
		std::map<FD, class Client> _clients;
	public:
		Server();
		~Server();

		void makeServerSocket(Config &Conf);
		void activateSocket(const Config &Conf);
		void queueInit();
		void changeEvents(std::vector<struct kevent> &changeList, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		int getSocketIndex(FD fd);
		
		void run(const Config &Conf);
		void readEventList(int eventNumber, struct kevent *eventList);
		void eventHandling(struct kevent &currEvent);
		void execute(Client &param);
		void executeCGI(Client &param);
};
#endif
