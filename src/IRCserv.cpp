#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"

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
	this->hostname = (std::string) (SERVER_PREFIX) + (std::string)(SERVER_SUFFIX);
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

int	countWords(const std::string &str)
{
	int count = 0;
	bool inWord = false;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ')
			inWord = false;
		else if (inWord == false)
		{
			inWord = true;
			count++;
		}
	}
	return count;
}


void	IRCserv::registeredAction(Client &client, std::string &buff)
{
	if (client.registered == 0)
	{
		size_t pos = buff.find("PASS");
		if (pos != std::string::npos && pos == 0)
		{
			if (buff.length() < 6 || buff[4] != ' ' || countWords(buff) != 2)
			{
				write(client.sock, "ERROR :Invalid password\n", 25);
				return;
			}
			std::string pass = buff.substr(pos + 5);
			while (pass[pass.length() - 1] == ' ')
				pass = pass.substr(0, pass.length() - 1);
			while (pass[0] == ' ')
				pass = pass.substr(1);
			if (countWords(pass) > 1)
			{
				write(client.sock, "ERROR :Invalid password\n", 25);
				return;
			}
			if (pass == password)
				client.registered = 1;
		}
	}
	else if (client.registered == 1)
	{
		size_t pos = buff.find("NICK");
		if (pos != std::string::npos && pos == 0)
		{
			bool useLabel = false;
			std::string temp = buff;
			if (temp.find("\nUSER") != std::string::npos)
				useLabel = true;
			if (useLabel)
			{
				temp = temp.substr(temp.find("USER"));
				buff.erase(buff.find("\nUSER"));
			}

			if (buff.length() < 6 || buff[4] != ' ' || countWords(buff) != 2)
			{
				write(client.sock, "ERROR :Invalid nickname\n", 25);
				return;
			}
			std::string nick = buff.substr(pos + 5);
			while (nick[0] == ' ')
				nick = nick.substr(1);
			while (nick[nick.length() - 1] == ' ')
				nick = nick.substr(0, nick.length() - 1);
			// let's check if nickname is valid
			if (countWords(nick) > 1)
			{
				write(client.sock, "ERROR :Invalid nickname\n", 25);
				return;
			}
			// let's check if nickname is already taken
			for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
			{
				if (it->second.nick == nick)
				{
					write(client.sock, "ERROR :Nickname is already taken\n", 33);
					return;
				}
			}
			client.nick = nick;
			client.registered = 2;
			if (useLabel)
				goto label;
		}
	}
	else if (client.registered == 2)
	{
		label:
		size_t pos = buff.find("USER");
		if (pos != std::string::npos && pos == 0)
		{
			if (buff.length() < 6 || buff[4] != ' ' || countWords(buff) != 5) // example: `USER guest 0 * :Ronnie`, where 0 is mode, * is realname and Ronnie Reagan is real name
			{
				write(client.sock, "ERROR :Invalid user\n", 21);
				return;
			}
			
			std::string user = buff.substr(pos + 5, buff.find(' ', pos + 5) - pos - 5);
			while (user[0] == ' ')
				user = user.substr(1);
			while (user[user.length() - 1] == ' ')
				user = user.substr(0, user.length() - 1);
			client.user = user;
			client.registered = 3;
			// let's get the host name
			client.hostname = buff.substr(buff.find(' ', pos + 5) + 1);
			client.hostname = client.hostname.substr(0, client.hostname.find(' '));
			std::cout << "Client registered: " << client.nick << " " << client.user << " " << client.hostname << std::endl;
			std::cout << "buff: " << buff << std::endl;
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
					int	tempFd = fds[i].fd;
					fds.erase(fds.begin() + i);
					clients.erase(tempFd);
					i--;
				}
				else if (len == 0)
				{
					std::cout << "Client disconnected" << std::endl;
					close(fds[i].fd);
					int	tempFd = fds[i].fd;
					fds.erase(fds.begin() + i);
					clients.erase(tempFd);
					i--;
				}
				else
				{
					buff[len] = 0;
					// remove tailing of limechat client and nc client
					if (buff[len - 1] == '\n')
						buff[len - 1] = 0;
					if (buff[len - 2] == '\r')
						buff[len - 2] = 0;
					std::string temp = buff;
					int tempStatus = clients[fds[i].fd].registered;
					if (clients[fds[i].fd].registered < 3)
						registeredAction(clients[fds[i].fd], temp);
					if (tempStatus == clients[fds[i].fd].registered && clients[fds[i].fd].registered != 3)
					{
						write(fds[i].fd, "ERROR :You have not registered\n", 31);
						continue;
					}
					else if (tempStatus == 3)
					{
						// here we can add application logic
						std::cout << "Received: " << buff << std::endl;
						handle_message(buff, clients[fds[i].fd]);
					}
				}
			}
		}
	}
}


void IRCserv::handle_message(char *msg, Client client)
{
	char *cmd;
	char *tmp = strdup(msg);
    cmd = strtok(tmp, " ");

    if (!strcmp("JOIN", cmd))
        this->parseChannelMessage(msg, client);
	else if (!strcmp("PASS", cmd) || !strcmp("USER", cmd))
		client.send_message(ERR_ALREADYREGISTERED(client.nick, this->getHostName()));
	else if (!strcmp("NICK", cmd))
	{
		char *nick = strtok(NULL, " ");
		if (nick)
			client.nick = std::string(nick);
	}
	else if (!strcmp("PRIVMSG", cmd))
	{
		this->parsePRIVMSG(msg, client);
	}
}

void IRCserv::parsePRIVMSG(char *msg, Client &client)
{
char *tmp;
(void) client;
tmp = strtok(msg, " ");
if (strcmp("PRIVMSG", tmp))
    return;
char *target = strtok(NULL, ":");
if (!target)
{
    client.send_message(":" + this->getHostName() + " 411 " + client.nick + " :No recipient given\r\n");
    return;
}
char *message = strtok(NULL, "");
if (!message)
{
    client.send_message(":" + this->getHostName() + " 412 " + client.nick + " :No text to send\r\n");
    return;
}

bool recipientFound = false;
for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->second.nick == target) {
		std::string msg_sent = client.nick + ":"+ message + "\r\n";
        it->second.send_message(message);
        recipientFound = true;
        break;
    }
}

if (!recipientFound) {
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if ((*it)->getName() == target) {
            if ((*it)->is_member(client)) {
                (*it)->send_message(client, message);
            } else {
                client.send_message(":" + this->getHostName() + " 404 " + client.nick + " " + (*it)->getName() + " :Cannot send to channel\r\n");
            }
            recipientFound = true;
            break;
        }
    }
}

if (!recipientFound) {
    client.send_message(":" + this->getHostName() + " 401 " + client.nick + " " + target + " :No such nick/channel\r\n");
}

std::cout << "target : " << target << std::endl;
std::cout << "message : " << message << std::endl;
	
}

void IRCserv::parseChannelMessage(char *msg, Client &client)
{
    char *tmp;
	(void) client;
    tmp = strtok(msg, " ");
    if (strcmp("JOIN", tmp))
        return;
    char *_channels = strtok(NULL, " ");
    if (_channels && *_channels)
    {
        char *chName;
        char *_keys = strtok(NULL, "");
        std::vector<std::string> keys;
        if (_keys)
            keys = split(((std::string)_keys), ' ');
        unsigned long i = 0;
        chName = strtok(_channels, ",");
        while (chName != NULL)
        {
			char *pass = NULL;
			std::string channelName = chName;
			if (channelName[0] == '#')
			{
				if (!channelName[1])
				{
					client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
					return;
				}
			}
			else 
			{
				if (i != 0)
				{
					channelName = "#" + channelName;
				}
				else if (channelName.compare("0")) 
				{
					client.send_message(ERR_BADCHANNELNAME(client.nick, this->getHostName(), channelName));
					return;
				}
			}
            if (i >= 0 && keys.size() > i)
				pass = (char *)keys[i].c_str();
			this->addNewChannel(channelName, pass, client);
            chName = strtok(NULL, ",");
            i++;
        }
        
    }
}

void IRCserv::parsePartMessage(char *msg, Client &client)
{
    char *tmp;
	(void) client;
    tmp = strtok(msg, " ");
    if (strcmp("PART", tmp))
        return;
    char *_channels = strtok(NULL, " ");
    if (_channels && *_channels)
    {
        char *chName;
        char *_reasons = strtok(NULL, "");
        std::vector<std::string> reasons;
        if (_reasons)
            reasons = split(((std::string)_reasons), ' ');
        unsigned long i = 0;
        chName = strtok(_channels, ",");
        while (chName != NULL)
        {
			char *pass = NULL;
			std::string channelName = chName;
			


            if (i >= 0 && reasons.size() > i)
				pass = (char *)reasons[i].c_str();
            chName = strtok(NULL, ",");
            i++;
        }
    }
}
std::string IRCserv::getHostName()
{
	return (this->hostname);
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