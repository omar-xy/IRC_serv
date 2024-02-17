#include "include/Server.hpp"
#include "include/Client.hpp"


int main(int ac, char **av)
{
	(void)av;
	if (ac != 3)
	{
		std::cerr << "Error: usage ./ircserv <port> <password>" << std::endl;
		exit(1);
	}
	Server server = Server(AF_INET, 5000, INADDR_ANY, av[2]);

	std::cout << "the end" << std::endl;
}