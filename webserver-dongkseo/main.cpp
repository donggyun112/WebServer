#include "../socket/socket.hpp"
#include "../utils/utils.hpp"
#include <string>
#include <iostream>

class WebServer
{
	public:
		WebServer(int port)
		{
			this->port = port;
			this->server = new Server(port);
		}

		void start()
		{
			while (true)
			{
				Client *client = server->accept();
				std::string request = client->read();
				std::string response = this->handle(request);
				client->write(response);
				client->close();
			}
		}

		std::string handle(std::string request)
		{
			std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
			response += "<!DOCTYPE html><html><head><title>WebServer</title></head><body><h1>Hello, WebServer!</h1></body></html>";
			return response;
		}
};


int main()
{

}