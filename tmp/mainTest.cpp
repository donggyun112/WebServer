#include "../Server/EchoServer.hpp"
#include "../Parse/Config.hpp"
#include "../utils/Error.hpp"

using namespace std;

int main(int ac, char **av)
{
	std::cout << __LINE__ << std::endl;
	Error::initializeError();
	std::cout << __LINE__ << std::endl;
	Config Conf(ac, av);
	std::cout << __LINE__ << std::endl;
	Server Server;
	std::cout << __LINE__ << std::endl;
	Server.makeServerSocket(Conf);
	std::cout << __LINE__ << std::endl;
	Server.queueInit(Conf);
	std::cout << __LINE__ << std::endl;
	Server.run(Conf);
}
