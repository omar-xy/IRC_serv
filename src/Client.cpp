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
    if (_name.empty())
        throw ApplicationException("Name cannot be empty");
    name = _name;
}

void Client::setUser(std::string _user)
{
    if (_user.empty())
        throw ApplicationException("User cannot be empty");
    username = _user;
}

bool Client::is_registred()
{
    return (!this->realname.empty() && !this->username.empty());
}

void Client::sendMessage(std::string msg)
{
    send(this->_sockfd, msg.c_str(), msg.length(), 0);

}

void Client::parseUsername(char *message)
{
    char *tmp;

    tmp = strtok(message, " ");
    if (strcmp(tmp, "USER"))
        return;
    tmp = strtok(NULL, " ");
    if (tmp && *tmp)
        this->setUser(tmp);
    tmp = strtok(NULL, " ");
    tmp = strtok(NULL, " ");
    if (!tmp || *tmp  != '*')
        return;
    tmp = strtok(NULL, ""); 
    if (tmp && *tmp)
        this->realname = tmp;
    std::cout << "realname : " << this->realname << std::endl;
    std::cout << "USERNAME : " << this->username << std::endl;
    // ELSE THROw MSG FEW PARAMS
}