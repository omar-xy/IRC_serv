#include "../headers/Client.hpp"
class Client;

Client::Client()
{
	this->sock = -1;
	this->registered = false;
	std::string nickname = "";
	std::string username = "";
}

Client::Client(int sock)
{
	this->sock = sock;
	this->registered = false;
}

void Client::send_messageCH(char *msg)
{
	write(this->sock, msg, strlen(msg));
}
void Client::send_message(std::string msg)
{
	write(this->sock, msg.c_str(), msg.length());
}

void Client::eraseInvitedChannel(std::string channelName)
{
	std::vector<std::string>::iterator it;
	for (it = this->invitedChannels.begin(); it < this->invitedChannels.end(); it++)
	{
		if (*it == channelName)
		{
			this->invitedChannels.erase(it);
			return;
		}
	}
}


void Client::eraseChannel(std::string channelName)
{
	std::vector<Channel *>::iterator it;
	for (it = this->_channels.begin(); it < this->_channels.end(); it++)
	{
		if ((*it)->getName() == channelName)
		{
			this->_channels.erase(it);
			return;
		}
	}
}

std::string	Client::getInvitedChannels()
{
	std::string str;
	for (size_t i = 0; i < this->invitedChannels.size(); i++)
	{
		str += this->invitedChannels[i];
		if (i != this->invitedChannels.size() - 1)
			str += ", ";
	}
	return str;
}


std::vector<Channel *> Client::getChannels()
{
	return this->_channels;
}

void Client::leaveAllChannels(std::string reason)
{
	std::vector<Channel*>::iterator it;
	for (it = this->_channels.begin(); it < this->_channels.end(); it++)
		(*it)->quitClient(*this, reason);
		
}

void Client::disconnect()
{

}

std::string Client::getIpAddress()
{
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	std::string ip_address;
	if (getpeername(this->sock, (struct sockaddr *)&client_addr, &addr_len) == 0) {
		char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
		ip_address = ip_str;
	}
	return ip_address;
}

Client::~Client()
{
	// std::cout << "Client disconnecteded and deconstructor called" << std::endl;
}


ClientErrMsgException::ClientErrMsgException(std::string msg, Client &client) throw() : _cmessage(msg), _client(client)
{
}

ClientErrMsgException::~ClientErrMsgException() throw()
{

}

const std::string ClientErrMsgException::getMessage() const
{
	return this->_cmessage;
}

bool Client::operator== (const Client &c)
{ 
	if (c.nick == this->nick) 
		return true; 
	return false; 
} 
bool Client::operator!= (const Client &c)
{ 
	return (!(this == &c));
} 