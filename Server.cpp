#include "include/Server.hpp"

void	exitError(int status, std::string err)
{
	if (status < 0)
	{
		perror(err.c_str());
		exit(1);
	}
}

void	exitError(int status, std::string err, int sock)
{
	if (status < 0)
	{
		close(sock);
		perror(err.c_str());
		exit(1);
	}
}

void	exitError(int status, std::string err, int sock, int client)
{
	if (status < 0)
	{
		close(sock);
		close(client);
		perror(err.c_str());
		exit(1);
	}
}

Server::Server(sa_family_t family, in_port_t port, in_addr_t _addr, std::string _passwd) : passwd(_passwd)
{
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = family;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(_addr);

	exitError((sock = socket(AF_INET, SOCK_STREAM, 0)), "socket()");
	int	opt = 1;
	exitError(setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)), "setsockopt()", sock);
	exitError(fcntl(sock, F_SETFL, O_NONBLOCK), "fcntl()", sock);
	exitError(bind(sock, (sockaddr *)&addr, sizeof(addr)), "bind()", sock);
	exitError(listen(sock, SOMAXCONN), "listen()", sock);
	fds.push_back((pollfd){sock, POLL_IN});
	handleClients();
}

void	Server::addClient()
{
	int	clientSock = accept(sock, NULL, NULL);

	exitError(clientSock, "accept()", sock);
	fds.push_back((pollfd){clientSock, POLL_IN});
	clients[clientSock] = Client();
}

bool	Server::isUniqueNick(std::string nick)
{
	if (!nick.size())
		return (false);
	for (std::unordered_map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->second.nick == nick)
			return (false);
	}
	return (true);
}

void	Server::handShake(int sock, std::string buff, unsigned int index)
{
	if (!(clients[sock].authLevel & 1) && buff.substr(0, 5) == "PASS " && buff.substr(5) == passwd)
		clients[sock].authLevel++;
	else if (!(clients[sock].authLevel & 2) && buff.substr(0, 5) == "NICK " && isUniqueNick(buff.substr(5)))
	{
		clients[sock].nick = buff.substr(5);
		clients[sock].authLevel = clients[sock].authLevel | 2;	
	}
	else if (!(clients[sock].authLevel & 4) && buff.substr(0, 5) == "USER ")
	{
		clients[sock].login = buff.substr(5);
		clients[sock].authLevel = clients[sock].authLevel | 4;
	}
	if ((clients[sock].authLevel == 7) && clients[sock].loggedIn == false)
	{
		std::string msg = ":localhost 001 cypher :Welcome to the server ";
		msg += clients[sock].nick;
		msg += "!\r\n";
		send(sock, msg.c_str(), msg.size(), 0);
		std::cout << "Client " << clients[sock].nick << " has connected!" << std::endl;
		clients[sock].loggedIn = true;
	}
}

void	Server::handleClients()
{
	while (1)
	{
		int res = poll(fds.data(), fds.size(), 0);
		exitError(res, "poll()", sock);
		if (fds[0].revents & POLL_IN)
			addClient();
		for (unsigned int i = 1; i < fds.size(); i++)
		{
			if (fds[i].revents & POLL_IN)
			{
				char	buff[512];
				ssize_t	len = recv(fds[i].fd, &buff, sizeof(buff) - 1, 0);
				if (len < 0)
				{
					perror("recv()");
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					i--;
				}
				else if (len == 0)
				{
					std::cout << "Client Disconnected!" << std::endl;
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					i--;
				}
				else
				{
					buff[len > 0 ? ((len > 1 && buff[len - 2] == '\r') ? len - 2 : len - 1) : len] = '\0';
					std::string tempBuff = buff;
					if (clients[fds[i].fd].authLevel != 7 && tempBuff.size())
						handShake(fds[i].fd, tempBuff, i);
				}
			}
		}
	}
}