#include "../socket/socket.hpp"

int main()
{
	Socket s("127.0.0.1", 80);
	s.autoActivate();
	
}
