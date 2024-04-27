#include "EchoServer.hpp"
#include "../Parse/Config.hpp"

int main(int ac, char **av)
{
	Config Conf(ac, av);
	Server Server;
	Server.makeServerSocket(Conf);
	Server.queueInit(Conf);
	Server.run(Conf);
}
