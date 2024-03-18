#include "../headers/Channel.hpp"

// Channel::Channel()
// {

// }

Channel::~Channel()
{

}

Channel::Channel(std::string name, std::string pass, int opFd)
{
    this->name = name;
    if (!pass.empty())
        this->pass = pass;
    this->fdOp = opFd;
}

std::string Channel::getName()
{
    return name;
}

void Channel::setName(std::string name)
{
    this->name = name;
}
