#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <vector>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>

class	Client
{
	private:
		int	socket;
		std::string login;
		std::string nick;
	public:
		Client(int	socket, std::string login, std::string nick);
};