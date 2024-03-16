#pragma once

#include "header.hpp"

class Client
{
	private:
		std::string		nick;
		std::string		user;
		bool			registered;
		int				sock;
	public:
		Client();
		Client(int sock);
};
