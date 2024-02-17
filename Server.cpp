#include "include/Server.hpp"

// 2.3.1 Message format in 'pseudo' BNF

class Message
{
public:
	std::string	prefix;
	std::string	command;
	std::vector<std::string> params;
	void	parse(std::string msg)
	{
		// \r\n
		size_t	pos = msg.find("\r\n");
		if (pos != std::string::npos)
			msg.erase(pos);
		// :prefix command params
		pos = msg.find(" ");
		if (msg[0] == ':')
		{
			prefix = msg.substr(1, pos - 1);
			msg.erase(0, pos + 1);
		}
		else
			prefix.clear();
		// command params
		pos = msg.find(" ");
		command = msg.substr(0, pos);
		msg.erase(0, pos + 1);
		// params
		while (msg.size())
		{
			if (msg[0] == ':')
			{
				params.push_back(msg.substr(1));
				break ;
			}
			pos = msg.find(" ");
			if (pos != std::string::npos)
			{
				params.push_back(msg.substr(0, pos));
				msg.erase(0, pos + 1);
			}
			else
			{
				params.push_back(msg);
				break ;
			}
		}
	}
	std::string	toString()
	{
		std::string ret;
		if (prefix.size())
			ret += ":" + prefix + " ";
		ret += command;
		for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); it++)
			ret += " " + *it;
		ret += "\r\n";
		return (ret);
	}
	void	clear()
	{
		prefix.clear();
		command.clear();
		params.clear();
	}
	void	printMessage()
	{
		std::cout << "Prefix: " << prefix << std::endl;
		std::cout << "Command: " << command << std::endl;
		for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); it++)
			std::cout << "Param: " << *it << std::endl;
	}
};




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

void	Server::handShake(int sock, Message msg)
{
	if (msg.command == "NICK")
	{
		if (msg.params.size() != 1)
			return ;
		if (isUniqueNick(msg.params[0]))
		{
			clients[sock].authLevel |= 1;
			clients[sock].nick = msg.params[0];
		}
	}
	if (msg.command == "USER")
	{
		if (msg.params.size() != 4)
			return ;
		clients[sock].authLevel |= 2;
		clients[sock].login = msg.params[0];
	}
	if (msg.command == "PASS")
	{
		if (msg.params.size() != 1)
			return ;
		if (msg.params[0] == passwd)
		{
			clients[sock].authLevel |= 4;
		}
	}
	if (clients[sock].authLevel == 7)
	{
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
				Message	msg;
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
					
					buff[len] = '\0';
					msg.parse(buff);
					if (clients[fds[i].fd].authLevel != 7)
						handShake(fds[i].fd, msg);
					if (clients[fds[i].fd].loggedIn)
					{
						if (msg.command == "PRIVMSG")
						{
							std::string msgSend = ":" + clients[fds[i].fd].nick + "!" + clients[fds[i].fd].login + " PRIVMSG " + msg.params[0] + " :" + msg.params[1] + "\r\n";
							for (std::unordered_map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
							{
								if (it->second.nick == msg.params[0] && it->second.loggedIn)
									send(it->first, msgSend.c_str(), msgSend.size(), 0);
							}
						}
					}
				}
			}
		}
	}
}