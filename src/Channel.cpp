#include "Channel.hpp"

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
    Server::addNewChannel(this);
}

std::string Channel::getName()
{
    return name;
}

void Channel::setName(std::string name)
{
    this->name = name;
}

void Channel::parseChannelMessage(char *msg, int cIndex)
{
    (void) cIndex;
    char *tmp;

    tmp = strtok(msg, " ");
    if (strcmp("JOIN", tmp))
        return;
    char *_channels = strtok(NULL, " ");
    std::cout << "channels : " << _channels << std::endl;
    if (_channels && *_channels)
    {
        char *chName;
        char *_keys = strtok(NULL, "");
        std::vector<std::string> keys;
        if (_keys)
        {
            std::cout << "keys : " << _keys << std::endl;
            keys = split(((std::string)_keys), ' ');
        }
        unsigned long i = 0;
        chName = strtok(_channels, ",");
        while (chName != NULL)
        {
            std::cout << "Creating channel "<< chName ;
            if (i >= 0 && keys.size() > i)
                std::cout << "with pass "<< keys[i] ;
            std::cout << std::endl;
            chName = strtok(NULL, ",");
            i++;
        }
        
    }

}