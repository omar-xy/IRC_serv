#include "Channel.hpp"

Channel::Channel()
{

}

Channel::~Channel()
{

}

std::string Channel::getName()
{
    return name;
}

void Channel::setName(std::string name)
{
    this->name = name;
}