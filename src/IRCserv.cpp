#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"
#include "../headers/replies.hpp"
#include <algorithm>

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

Client	*IRCserv::isClientExisiting(std::string name)
{
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->second.nick == name)
			return &it->second;
	}
	return NULL;
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

bool	IRCserv::isValidNick(std::string nick)
{
	for (size_t i = 0; i < nick.length(); i++)
	{
		if (isspace(nick[i]))
			return false;
	}
	if (nick[0] == ':' || nick[0] == '#')
		return false;
	return true;
}

std::string IRCserv::removeTail(std::string buff)
{
	while (buff[buff.length() - 1] == '\n' || buff[buff.length() - 1] == '\r')
		buff = buff.substr(0, buff.length() - 1);
	
	return buff;
}

void	IRCserv::registeredAction(Client &client, std::string &buff)
{
	if (client.nick == "MyBot")
	{
    	client.registered = 3; 
    	return;
    }
	std::string temp = ParseBuffToRegister(buff);
	if (client.registered == 0)
	{
		std::string nickTemp;
		size_t pos = temp.find("NICK");
		if (pos != std::string::npos)
		{
			nickTemp = temp.substr(pos);
			temp = removeTail(temp.substr(0, pos));
		}
		if (getFirstWord(temp) == "PASS")
		{
			std::string pass = temp.substr(5);
			if (pass == this->password)
			{
				client.registered = 1;
				if (nickTemp.length())
					registeredAction(client, nickTemp);
			}
			else
			{
				client.send_message(ERR_PASSWDMISMATCH(std::string("unknown"), this->getHostName()));
				return;
			}
		}
		else
			return;
	}
	if (client.registered == 1)
	{
		std::string userTemp;
		size_t pos = temp.find("USER");
		if (pos != std::string::npos)
		{
			userTemp = temp.substr(pos);
			temp = removeTail(temp.substr(0, pos));
		}
		if (getFirstWord(temp) == "NICK")
		{
			std::string nick = temp.substr(5);
			if (nick.empty())
			{
				client.send_message(ERR_NONICKNAMEGIVEN(std::string("unknown"), this->getHostName()));
				return;
			}
			if (!isValidNick(nick))
			{
				client.send_message(ERR_ERRONEUSNICKNAME(std::string("unknown"), this->getHostName()));
				return;
			}
			if (isClientExisiting(nick))
			{
				client.send_message(ERR_NICKNAMEINUSE(std::string("unknown"), this->getHostName()));
				return;
			}
			client.nick = nick;
			client.registered = 2;
			if (userTemp.length())
				registeredAction(client, userTemp);		
		}
		else
			return;
	}
	if (client.registered == 2)
	{
		if (getFirstWord(temp) == "USER")
		{
			char *first= strtok((char *)temp.c_str(), " ");
			(void )first;
			char *user = strtok(NULL, " ");
			char *mode = strtok(NULL, " ");
			char *unused = strtok(NULL, " ");
			char *realname = strtok(NULL, "");
			if (!realname)
			{
				client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
				return;
			}
			client.user = user;
			client.mode = mode;
			client.hostname = unused;
			client.realname = realname;
			client.registered = 3;
			client.send_message(RPL_WELCOME(client.nick, this->getHostName()));
			client.send_message(RPL_YOURHOST(client.nick, this->getHostName()));
			client.send_message(RPL_CREATED(client.nick, this->getHostName()));
			client.send_message(RPL_MYINFO(client.nick, this->getHostName()));
		}
		else
			return;
	}

}

void	IRCserv::loop()
{
	while (1)
	{
		int ret = poll(fds.data(), fds.size(), -1);
		debug("Poll failed", ret);
		if (fds[0].revents & POLLIN)
		{
			addClient();
		}
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
					strcpy(buff, removeTail(buff).c_str());
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
						std::cout << "Received by `" << clients[fds[i].fd].nick << "`: " << buff << std::endl;
						handle_message(buff, clients[fds[i].fd]);
					}
				}
			}
		}
	}
}


void IRCserv::handle_message(char *msg, Client &client)
{

	char *cmd;
	char *tmp = strdup(msg);
    cmd = strtok(tmp, " ");

    if (!strcmp("JOIN", cmd))
        this->parseChannelMessage(msg, client);
	else if (!strcmp("PART", cmd))
        this->parsePartMessage(msg, client);
	else if (!strcmp("QUIT", cmd))
        this->parseQuitMessage(msg, client);
	else if (!strcmp("PASS", cmd) || !strcmp("USER", cmd))
		client.send_message(ERR_ALREADYREGISTERED(client.nick, this->getHostName()));
	else if (!strcmp("NICK", cmd))
	{
		char *nick = strtok(NULL, " ");
		if (nick && !isClientExisiting(nick))
			client.nick = std::string(nick);
	}
	else if (!strcmp("PRIVMSG", cmd))
		this->parsePRIVMSG(msg, client);
	else if (!strcmp("MODE" , cmd))
		this->handleMode(msg, client);
	else if (!strcmp("INVITE", cmd))
		this->handleInvite(msg, client);
	else if (!strcmp("KICK", cmd))
		this->handleKick(msg, client);
	else if (!strcmp("TOPIC", cmd))
		this->handleTopic(msg, client);
	// else if (!strcmp("BOT", cmd))
	// 	this->handleBot(msg, client);
}

void IRCserv::handleTopic(char *msg, Client &client)
{
	char *part = strtok(msg, " ");
	if (strcmp("TOPIC", part))
		return;
	char *channel = strtok(NULL, " ");
	char *topic = strtok(NULL, "");
	if (topic && topic[0] == ':')
		topic++;
	else if (topic && topic[0] != ':')
		topic = strtok(topic, " ");
	if (!channel)
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
	if (!topic)
	{
		if (ch->isTopicSet && ch->isFdOperator(client.sock))
		{
			ch->setTopic("");
			client.send_message(RPL_TOPIC(client.nick, this->getHostName(), channel, ch->getTopicNickSetter(), ch->getTopic()));
		}
		else if (!ch->isTopicSet)
		{
			ch->setTopic("");
			client.send_message(RPL_TOPIC(client.nick, this->getHostName(), channel, ch->getTopicNickSetter(), ch->getTopic()));
		}
		else
			client.send_message(ERR_CHANOPRIVSNEEDED(client.nick, this->getHostName(), channel));
	}
	else
	{
		if (ch->isTopicSet && ch->isFdOperator(client.sock))
		{
			ch->setTopic(topic);
			client.send_message(RPL_TOPIC(client.nick, this->getHostName(), channel, ch->getTopicNickSetter(),ch->getTopic()));
		}
		else if (!ch->isTopicSet)
		{
			ch->setTopic("");
			client.send_message(RPL_TOPIC(client.nick, this->getHostName(), channel, ch->getTopicNickSetter(), ch->getTopic()));
		}
		else
			client.send_message(ERR_CHANOPRIVSNEEDED(client.nick, this->getHostName(), channel));
	}
}

void IRCserv::handleKick(char *msg, Client &client)
{
	char *part = strtok(msg, " ");
	if (strcmp("KICK", part))
		return;
	char *channel = strtok(NULL, " ");
	char *nick = strtok(NULL, " ");
	char *reason = strtok(NULL, "");
	if (!channel || !nick)
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
	if (!ch->isNickInChannel(nick))
	{
		client.send_message(ERR_USERNOTINCHANNEL(this->getHostName(), channel));
		return;
	}
	if (!ch->isFdOperator(client.sock))
	{
		client.send_message(ERR_NOTOP(this->getHostName(), channel));
		return;
	}

	Client *kicked = NULL;
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->second.nick == nick)
		{
			kicked = &it->second;
			break;
		}
	}
	if (!kicked)
	{
		client.send_message(ERR_NOSUCHNICK(this->getHostName(), channel, nick));
		return;
	}
	// send reply to kick client from channel
	std::string reasonStr = reason;
	if (reasonStr.size() > 0 && reasonStr[0] == ':')
		reasonStr = reasonStr.substr(1);
	ch->send_message(RPL_KICK(client.nick, client.user, this->getHostName(), channel, kicked->nick, reasonStr));
	kicked->eraseChannel(ch->getName());
	ch->eraseClient(*kicked);
}

void	IRCserv::handleInvite(char *msg, Client &client)
{
	char *tmp;
	tmp = strtok(msg, " ");
	if (strcmp("INVITE", tmp))
		return;
	char *nick = strtok(NULL, " ");
	char *channel = strtok(NULL, " ");
	if ((!nick || !strcmp(nick, ":"))&& !channel)
	{
		client.send_message(RPL_INVITELIST(client.nick, this->getHostName(), client.getInvitedChannels()));
		client.send_message(RPL_ENDOFINVITELIST(client.nick, this->getHostName()));
		return;
	}
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
	inviteClinetToChannel(*invited, *ch, client);
}

void IRCserv::inviteClinetToChannel(Client &invitedClient, Channel &channel, Client &client)
{
	// invitedClient.invitedChannels.push_back(channel.getName());
	// check if client is already invited
	std::vector<std::string>::iterator it = std::find(invitedClient.invitedChannels.begin(), invitedClient.invitedChannels.end(), channel.getName());
	if (it == invitedClient.invitedChannels.end())
		invitedClient.invitedChannels.push_back(channel.getName());
	bool isAreadyInvited = false;
	for (size_t i = 0; i < channel.clientsInvited.size(); i++)
	{
		if (channel.clientsInvited[i].nick == invitedClient.nick)
		{
			isAreadyInvited = true;
			break;
		}
	}
	if (!isAreadyInvited)
		channel.clientsInvited.push_back(invitedClient);
	invitedClient.send_message(RPL_INVITING(client.nick, this->getHostName(), invitedClient.nick, channel.getName()));
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
            keys = split(((std::string)_keys), ' ');// no need to type_cast..
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


void IRCserv::parseQuitMessage(char *msg, Client &client)
{
    char *tmp;
	(void) client;
    tmp = strtok(msg, " ");
    if (strcmp("QUIT", tmp))
        return;
	std::string reason = strtok(NULL, "");
	if (reason.size() > 0 && reason[0] == ':')
		reason = reason.substr(1);
    client.leaveAllChannels(reason);
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
		char *_keys = strtok(NULL, "");
        char *chName;
        unsigned long i = 0;
        chName = strtok(_channels, ",");
        while (chName != NULL)
        {
			std::cout <<"parting channel  : "<< chName << std::endl;
			this->partChannel(std::string(chName), _keys, client);
            chName = strtok(NULL, ",");
            i++;
        }
    }
	else client.send_message(ERR_NEEDMOREPARAMS(client.nick, hostname));
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
