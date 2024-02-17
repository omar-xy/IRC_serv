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
	public:
		Client();
		int			authLevel;
		bool		loggedIn;
		std::string nick;
		std::string login;
};