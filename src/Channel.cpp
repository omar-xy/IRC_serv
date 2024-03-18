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
    this->clients.push_back(client);
    this->topic_nicksetter = client.nick;
    this->topic_usersetter = client.user;
    this->topic_set_timestamp = time(NULL);
}

std::vector<Client> Channel::getClients()
{
    return (this->clients);
}

std::string Channel::getName()
{
    return name;
}

std::string Channel::getTopicUserSetter()
{
    return this->topic_usersetter;
}
std::string Channel::getTopicNickSetter()
{
    return this->topic_nicksetter;
}
std::string Channel::getTopic()
{
    return this->topic;
}

std::string Channel::getTopicTimestamp()
{
    std::stringstream ss;
    std::string str;

    ss << this->topic_set_timestamp;
    ss >> str;
    return (str);
}

void Channel::setName(std::string name)
{
    this->name = name;
}

Channel *IRCserv::isChannelExisiting(std::string name)
{
	std::vector<Channel *>::iterator it;
	for (it = this->channels.begin(); it < this->channels.end(); it++)
	{
		Channel *c = *it;
		if (name.compare((*c).getName()) == 0)
			return c;
	}
	return NULL;
}

std::string Channel::getListClients()
{
    std::string str;
    std::vector<Client>::iterator it;
    for (it = this->clients.begin();it < this->clients.end();it++)
    {
        if (it != this->clients.begin())
            str += " ";
        if ((it)->sock == this->fdOp)
            str += "@";
        str += (it)->nick;
    }
    std::cout << "Clients list of " << this->name << "  : " << str << std::endl;
    return str;
}

void Channel::addClient(Client &client)
{
    this->clients.push_back(client);
}

void Channel::rpl_join(Client &client)
{
    std::vector<Client>::iterator it;
    for (it = this->clients.begin();it < this->clients.end();it++)
    {
        it->send_message(RPL_JOIN(client.nick, client.user, this->getName(), client.getIpAddress()));
    }
}

void IRCserv::addNewChannel(std::string name,char *pass, Client client)
{
    Channel *channel = isChannelExisiting(name);
    if (!channel)
    {
        channel = new Channel(name, pass,client);
        this->channels.push_back(channel);
        client.send_message(RPL_JOIN(client.nick, client.user, name, client.getIpAddress()));
        client.send_message(RPL_MODEIS(name, this->getHostName(), "+p"));
        client.send_message(RPL_NAMREPLY(this->getHostName(), channel->getListClients(), channel->getName(), client.nick));
        client.send_message(RPL_ENDOFNAMES(this->getHostName(), client.nick, name));
        // std::cout << "Channel created and added" << std::endl;
    }else
    {
        std::cout << "Channel existing" << std::endl;
        //if mode is good
        channel->addClient(client);
        client.send_message(RPL_JOIN(client.nick, client.user, name, client.getIpAddress()));
        client.send_message(RPL_TOPICDISPLAY(this->getHostName(), client.nick, name, channel->getTopic()));
        client.send_message(RPL_TOPICWHOTIME((*channel).getTopicNickSetter(), channel->getTopicTimestamp(),
            client.nick, this->getHostName(), (*channel).getName()));
        client.send_message(RPL_NAMREPLY(this->getHostName(), channel->getListClients(), channel->getName(), client.nick));
        client.send_message(RPL_ENDOFNAMES(this->getHostName(), client.nick, name));
        channel->rpl_join(client);
    }
}