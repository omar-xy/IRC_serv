#include "../headers/Channel.hpp"

// Channel::Channel()
// {

// }

Channel::~Channel()
{

}

Channel::Channel(std::string name, char *pass, Client client)
{
    this->name = name;
    if (pass)
        this->pass = pass;
    this->fdOp = client.sock;
}

std::string Channel::getName()
{
    return name;
}

void Channel::setName(std::string name)
{
    this->name = name;
}

bool IRCserv::isChannelExisiting(std::string name)
{
	std::vector<Channel>::iterator it;
	for (it = this->channels.begin(); it < this->channels.end(); it++)
	{
		Channel c = *it;
		if (name.compare(c.getName()) == 0)
			return true;
	}
	return false;
}

void IRCserv::addNewChannel(std::string name,char *pass, Client client)
{
    if (!isChannelExisiting(name))
    {
        this->channels.push_back(Channel(name, pass,client));
        std::cout << "Channel created and added" << std::endl;
    }else
        std::cout << "Channel existing" << std::endl;
}