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

		void send_message(std::string msg);
		void send_message(char *msg);
		std::string getIpAddress();

		std::string getFullname();


};
