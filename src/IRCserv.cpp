#include "../headers/IRCserv.hpp"

IRCserv::IRCserv(std::string port, std::string password)
{
	std::stringstream ss(port);
	if (!(ss >> this->port))
		throw std::invalid_argument("Invalid port");
	if (this->port < 1024 || this->port > 65535)
		throw std::invalid_argument("Invalid port");
	if (!password.length())
		throw std::invalid_argument("Invalid password");
	this->password = password;
	std::cout << "Server started on port " << this->port << " with password " << this->password << std::endl;
	init();
}

void	IRCserv::init()
{
	struct sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		throw std::runtime_error("Socket creation failed");
	fcntl(sock, F_SETFL, O_NONBLOCK);
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Bind failed");
	if (listen(sock, SOMAXCONN) < 0)
		throw std::runtime_error("Listen failed");
	std::cout << "Server listening on port " << port << std::endl;
	fds.push_back((pollfd){sock, POLLIN, 0});
	loop();
}

void	IRCserv::debug(std::string msg, int status)
{
	if (status < 0)
		throw std::runtime_error(msg);
}

void	IRCserv::registeredAction(Client &client, const std::string &buff)
{
	if (client.registered == 0)
	{
		size_t pos = buff.find("PASS");
		if (pos != std::string::npos)
		{
			pos += 5;
			std::string pass = buff.substr(pos);
			if (pass.size() >= 2)
			{
				pass.pop_back();
			}	
				
			
			std::cout << "Password: " << pass << ", len:" << pass.length()  << std::endl;
			if (pass == password)
				client.registered = 1;
			else
				std::cerr << "Invalid password" << std::endl;
		}
		else
			return ;
	}
	else if (client.registered == 1)
	{
		size_t pos = buff.find("USER");
		if (pos != std::string::npos)
		{
			pos += 5;
			std::string user = buff.substr(pos, buff.find("\r\n", pos) - pos);
			for (size_t i = 0; i < clients.size(); i++)
			{
				if (clients[i].user == user)
				{
					std::string response = "433 * " + user + " :Nickname is already in use\r\n";
					send(client.sock, response.c_str(), response.size(), 0);
					return ;
				}
			}
			client.user = user;
			client.registered = 2;
		}
	}
	else if (client.registered == 2)
	{
		size_t pos = buff.find("NICK");
		if (pos != std::string::npos)
		{
			pos = buff.find(" ", pos);
			if (pos == std::string::npos)
				return ;
			pos++;
			size_t tempPos = buff.find(" ", pos);
			if (tempPos == std::string::npos)
				return ;
			std::string nick = buff.substr(pos, tempPos - pos);
			pos = tempPos;
			pos = buff.find(" ", pos);
			if (pos == std::string::npos)
				return ;
			pos++;
			pos = buff.find(" ", pos);
			if (pos == std::string::npos)
				return ;
			client.nick = nick;
			client.registered = 3;
		}
	}			
}

void	IRCserv::loop()
{
	while (1)
	{
		int ret = poll(fds.data(), fds.size(), -1);
		debug("Poll failed", ret);
		if (fds[0].revents & POLLIN)
			addClient();
		for (size_t i = 1; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				char buff[512];
				ssize_t len = recv(fds[i].fd, buff, sizeof(buff) - 1, 0);
				if (len < 0)
				{
					std::cerr << "Recv failed" << std::endl;
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					clients.erase(fds[i].fd);
					i--;
				}
				else if (len == 0)
				{
					std::cout << "Client disconnected" << std::endl;
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					clients.erase(fds[i].fd);
					i--;
				}
				else
				{
					buff[len] = 0;
					int tempStatus = clients[fds[i].fd].registered;
					if (clients[fds[i].fd].registered < 3)
						registeredAction(clients[fds[i].fd], buff);
					if (tempStatus == clients[fds[i].fd].registered && clients[fds[i].fd].registered != 3)
					{
						write(fds[i].fd, "ERROR :You have not registered\n", 31);
						continue;
					}
					else
					{
						// here we can add application logic
						std::cout << "Received: " << buff << std::endl;
					}
				}
			}
		}
	}
}

void 	IRCserv::addClient()
{
	int clientSock = accept(sock, NULL, NULL);
	debug("Accept failed", clientSock);
	fcntl(clientSock, F_SETFL, O_NONBLOCK);
	fds.push_back((pollfd){clientSock, POLLIN, 0});
	std::cout << "New client connected" << std::endl;
	clients[clientSock] = Client(clientSock);
	clients[clientSock].registered = 0;

}