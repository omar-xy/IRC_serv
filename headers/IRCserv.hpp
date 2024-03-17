#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Client.hpp"
#include <sys/poll.h>
#include <unordered_map>

std::vector<std::string> split(const std::string &s, char delimiter);

class	Client;

class IRCserv {
	private:
		std::string			password;
		int					port;
		int					sock;
		std::vector<pollfd>	fds;
		std::unordered_map<int, Client>	clients;		
		void handle_message(char *msg, Client client);
        void parseChannelMessage(char *msg, Client client);



	public:
		IRCserv(std::string port, std::string password);
		void	init();
		void	debug(std::string msg, int status);
		void	loop();
		void 	addClient();
		void	registeredAction(Client &client, const std::string &buff);
};
