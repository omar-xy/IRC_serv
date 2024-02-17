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
#include "Client.hpp"
#include <unordered_map>

// struct sockaddr_in server;
// server.sin_family = AF_INET;
// server.sin_port = htons(8080);
// server.sin_addr.s_addr = INADDR_ANY;
// sa_family_t -> server.sin_family 8 bits	
// in_port_t -> server.sin_port 16 bits
// s_addr -> server.sin_addr.s_addr 32 bits
class Server 
{
	public:
		Server(sa_family_t family, in_port_t port, in_addr_t addr, std::string _passwd);
		void	handleClients();
		void	addClient();
		void	handShake(int sock, std::string buff, unsigned int index);
		bool	isUniqueNick(std::string nick);
	private:
		int sock;
		std::string passwd;
		struct sockaddr_in addr;
		std::vector<pollfd>	fds;
		std::unordered_map<int, Client> clients;
};