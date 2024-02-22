#include "Client.hpp"

Client::Client() 
{
}
Client::Client(int fd) 
{
    this->_sockfd = fd;
}

Client::~Client()
{

}

std::string Client::getName()
{
    return name;
}
std::string Client::getUser()
{
    return name;
}

void Client::setName(std::string _name)
{
    name = _name;
}

void Client::setUser(std::string _user)
{
    user = _user;
}

bool Client::is_logged()
{
    return (!this->name.empty() && !this->user.empty());
}

void Client::sendMessage(std::string msg)
{
    send(this->_sockfd, msg.c_str(), msg.length(), 0);

}