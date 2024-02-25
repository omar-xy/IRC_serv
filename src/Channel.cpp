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
        if (_keys)
            std::cout << "keys : " << _keys << std::endl;
        std::vector<std::string> keys = split(((std::string)_keys), ' ');
        int i = 0;
        chName = strtok(_channels, ",");
        while (chName != NULL)
        {
            std::cout << "Creating channel "<< chName << std::endl;
            // if (i >= 0 && keys)
            chName = strtok(NULL, ",");
        }
        
    }

}