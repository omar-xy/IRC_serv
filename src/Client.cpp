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