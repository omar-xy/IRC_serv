#include "../headers/header.hpp"
#include "../headers/IRCserv.hpp"

int main(int ac, char **av) {
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv [port] [password]" << std::endl;
		return 1;
	}
	try
	{
		IRCserv irc(av[1], av[2]);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}	
	return 0;
}
