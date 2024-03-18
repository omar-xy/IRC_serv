#pragma once

#include "header.hpp"

class Client
{
	public:
		std::string		nick;
		std::string		user;
		std::string		hostname;
		int			registered;
		int				sock;
		Client();
		Client(int sock);

		void send_message(char *msg);

};
