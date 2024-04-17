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


std::string ParseBuffToRegister(std::string &buff)
{
	// parse buff we will remove all extra spaces except for those comme after :
	// we will also remove the : and leave everything after it
	std::string temp;
	bool inWord = false;
	bool inColon = false;

	for (size_t i = 0; i < buff.length(); i++)
	{
		if (isspace(buff[i]) && inWord == false)
			continue;
		else if (buff[i] == ':')
		{
			inColon = true;
			continue;
		}
		else if (isspace(buff[i])&& inColon == true)
		{
			temp += buff[i];
			continue;
		}
		else if (buff[i] == ' ')
			inWord = false;
		else
			inWord = true;
		temp += buff[i];
	}
	return temp;
}

std::string getFirstWord(std::string str)
{
	std::string temp;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (isspace(str[i]))
			break;
		temp += str[i];
	}
	return temp;
}

void	IRCserv::registeredAction(Client &client, std::string &buff)
{
	std::string temp = ParseBuffToRegister(buff);
	if (client.registered == 0)
	{
		size_t pos = temp.find("PASS");
		if (pos != std::string::npos && pos == 0)
		{
			if (temp.length() < 6 || !isspace(temp[4]))
			{
				client.send_message(ERR_PASSWDMISMATCH(client.nick, this->getHostName()));
				return;
			}
			std::string pass = temp.substr(pos + 5);
			if (pass == password)
				client.registered = 1;
			else
				client.send_message(ERR_PASSWDMISMATCH(client.nick, this->getHostName()));
		}
		else
			client.send_message(ERR_PASSWDMISMATCH(client.nick, this->getHostName()));
	}
	else if (client.registered == 1)
	{
		size_t pos = temp.find("NICK");
		if (pos != std::string::npos && pos == 0)
		{
			bool useLabel = false;
			if (temp.find("\nUSER") != std::string::npos)
				useLabel = true;
			if (useLabel)
			{
				temp = temp.substr(temp.find("USER"));
				buff.erase(buff.find("\nUSER"));
			}
			if (temp.length() < 6 || !isspace(temp[4]) || countWords(temp) != 2)
			{
				client.send_message(ERR_NONICKNAMEGIVEN(client.nick, this->getHostName()));
				return;
			}

			std::string nick = temp.substr(pos + 5);
			if (nick.find(' ') != std::string::npos)
			{
				client.send_message(ERR_ERRONEUSNICKNAME(client.nick, this->getHostName()));
				return;
			}
			for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
			{
				if (it->second.nick == nick)
				{
					client.send_message(ERR_NICKNAMEINUSE(client.nick, this->getHostName()));
					return;
				}
			}
			client.nick = nick;
			client.registered = 2;
			if (useLabel)
				goto label;
		}
		else
			client.send_message(ERR_NONICKNAMEGIVEN(client.nick, this->getHostName()));
	}
	else if (client.registered == 2)
	{
		label:
		size_t pos = temp.find("USER");
		if (pos != std::string::npos && pos == 0)
		{
			if (temp.length() < 6 || !isspace(temp[4]) || countWords(temp) != 5)
			{
				client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
				return;
			}
			std::string user = getFirstWord(temp.substr(pos + 5));
			std::string mode = getFirstWord(temp.substr(temp.find(user) + user.length()));
			std::string realname = temp.substr(temp.find(mode) + mode.length());
			if (realname[0] == ':')
				realname = realname.substr(1);
			client.user = user;
			client.mode = mode;
			client.realname = realname;
			client.registered = 3;			


			client.send_message(RPL_WELCOME(client.nick, this->getHostName()));
		}
		else
			client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
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
	else if (!strcmp("MODE" , cmd))
		this->handleMode(msg, client);
	else if (!strcmp("INVITE", cmd))
		this->handleInvite(msg, client);
	else
	{
		std::cout << "Unknown command: " << cmd << std::endl;
	}
}

void	IRCserv::handleInvite(char *msg, Client &client)
{
	char *tmp;
	tmp = strtok(msg, " ");
	if (strcmp("INVITE", tmp))
		return;
	char *nick = strtok(NULL, " ");
	char *channel = strtok(NULL, " ");
	if (!nick || !channel)
	{
		client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
		return;
	}
	Channel *ch = isChannelExisiting(channel);
	if (!ch)
	{
		client.send_message(ERR_NOSUCHCHANNEL(client.nick, this->getHostName(), channel));
		return;
	}
	if (!ch->isClientOnChannel(client))
	{
		client.send_message(ERR_NOTONCHANNEL(this->getHostName(), channel));
		return;
	}
	if (ch->isInviteOnly() && !ch->isFdOperator(client.sock))
	{
		client.send_message(ERR_CHANOPRIVSNEEDED(client.nick, this->getHostName(), channel));
		return;
	}
	if (ch->isNickInChannel(nick))
	{
		client.send_message(ERR_USERONCHANNEL(client.nick, this->getHostName(), nick));
		return;
	}
	Client *invited = NULL;
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->second.nick == nick)
		{
			invited = &it->second;
			break;
		}
	}
	if (!invited)
	{
		client.send_message(ERR_NOSUCHNICK(client.nick, this->getHostName(), nick));
		return;
	}
	ch->addInvited(*invited);
	invited->send_message(RPL_INVITING(client.nick, this->getHostName(), nick, channel));
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
