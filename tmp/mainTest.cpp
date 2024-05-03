#include "EchoServer.hpp"
#include "../Parse/Config.hpp"

using namespace std;

int main(int ac, char **av)
{
	Config Conf(ac, av);

	cout << "Jackson pollack dick " << Conf[1].getPath() << std::endl;
	// Server Server;
	// Server.makeServerSocket(Conf);
	// Server.queueInit(Conf);
	// Server.run(Conf);
}
