#include "../Server/WebServer.hpp"
#include "../Parse/Config.hpp"
#include "../utils/Error.hpp"
#include <signal.h>

int main(int ac, char **av)
{
	Error::initializeError();
	Config Conf(ac, av);
	Server Server;
	Server.makeServerSocket(Conf);
	Server.queueInit(Conf);
	Server.run(Conf);
}
