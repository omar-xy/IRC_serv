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

Server::Server(sa_family_t family, in_port_t port, in_addr_t _addr)
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
	clients.push_back((pollfd){sock, POLL_IN});
	handleClients();
}

void	Server::handShake()
{
	int	clientSock = accept(sock, NULL, NULL);

	exitError(clientSock, "accept()", sock);

	char	buff[512];

	memset(&buff, 0, sizeof(buff));
	ssize_t	len = recv(clientSock, &buff, sizeof(buff) - 1, 0);

	if (len < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
			exitError(len, "recv()", sock, clientSock);

	buff[len] = '\0';

	std::cout << "Deb[we receive]: " << buff << std::endl; 
	clients.push_back((pollfd){clientSock, POLL_IN});
	
}

void	Server::handleClients()
{
	while (1)
	{
		int res = poll(clients.data(), clients.size(), 0);
		exitError(res, "poll()", sock);
		if (clients[0].revents & POLL_IN)
			handShake();
		for (unsigned int i = 1; i < clients.size(); i++)
		{
			if (clients[i].revents & POLL_IN)
			{
				char	buff[512];
				ssize_t	len = recv(clients[i].fd, &buff, sizeof(buff) - 1, 0);
				if (len < 0)
				{
					perror("recv(_)");
					close(clients[i].fd);
					clients.erase(clients.begin() + i);
					i--;
				}
				else if (len == 0)
				{
					std::cout << "Client Disconnected!" << std::endl;
					close(clients[i].fd);
					clients.erase(clients.begin() + i);
					i--;
				}
				else
				{
					buff[len] = '\0';
					std::cout << "Deb[we receive after success]: " << buff << std::endl; 
				}
			}
		}
	}
}