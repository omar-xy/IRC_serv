#include "../headers/Client.hpp"
class Client;

Client::Client()
{
	this->sock = -1;
	this->registered = false;
	std::string nickname;
	std::string username;
}

Client::Client(int sock)
{
	this->sock = sock;
	this->registered = false;
}

void Client::send_message(char *msg)
{
	write(this->sock, msg, strlen(msg));
}
void Client::send_message(std::string msg)
{
	write(this->sock, msg.c_str(), msg.length());
}

void Client::leaveAllChannels()
{
	// std::vector<Channel>::iterator it;
	// for (it = this->_channels.begin(); it < this->_channels.end(); it++)
	// {
	// 	it->
	// }
		
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
	std::cout << "Client disconnecteded and deconstructor called" << std::endl;
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

