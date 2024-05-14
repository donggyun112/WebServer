#include "../Server/EchoServer.hpp"
#include "../Parse/Config.hpp"
#include "../utils/Error.hpp"

using namespace std;

int main(int ac, char **av)
{
	Error::initializeError();
	Config Conf(ac, av);
	Server Server;
	Server.makeServerSocket(Conf);
	Server.queueInit(Conf);
	Server.run(Conf);
}
