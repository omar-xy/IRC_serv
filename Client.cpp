#include "headers/Client.hpp"

Client::Client()
{
	this->sock = -1;
	this->registered = false;
}

Client::Client(int sock)
{
	this->sock = sock;
	this->registered = false;
}