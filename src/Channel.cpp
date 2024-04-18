#include "../headers/Channel.hpp"

// Channel::Channel()
// {

// }

Channel::~Channel()
{
    this->_isOperator = false;

}

void Channel::eraseClient(Client &client)
{
    std::vector<Client>::iterator it;
    for (it = this->clients.begin(); it < this->clients.end(); it++)
    {
        if (it->nick == client.nick)
        {
            this->clients.erase(it);
            return;
        }
    }
}

Channel::Channel(std::string name, char *pass, Client &client, std::string srv_hst)
{
    if (!this->_isChannelNameValid(name))
        throw ClientErrMsgException(ERR_BADCHANNELNAME(client.nick, srv_hst, name), client);
    this->name = name;
    
    if (pass)
    {
        this->isPasswordSet = true;
        this->pass = pass;
    }
    else this->isPasswordSet = false;
    this->addClient(client, pass);
    this->fdOps.push_back(client.sock);
    this->topic_nicksetter = client.nick;
    this->topic_usersetter = client.user;
    this->topic_set_timestamp = time(NULL);
    this->srv_hostname = srv_hst;
}

bool Channel::_isChannelNameValid(std::string name)
{
    for (size_t i = 0; i < name.length(); i++)
    {
        if ((i != 0 && name[i] == '#') || name[i] == ',' || name[i] == ' '
            || name[i] == '\r' || name[i] == '\n')
            return false;
    }
    return true;    
}

std::vector<Client> Channel::getClients()
{
    return (this->clients);
}

// Client &Channel::getAclient(std::string nickname)
// {
//     std::vector<Client>::iterator it;
//     for (it = this->clients.begin(); it < this->clients.end(); it++)
//     {
//         if (nickname == it->nick)
//         {
//             return (*it);
//         }
//     }
// }

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

bool Channel::isFdOperator(int fd)
{
    std::vector<int>::iterator it;
    for (it = this->fdOps.begin(); it < this->fdOps.end(); it++)
    {
        if (*it == fd)
            return true;
    }
    return false;
}


void Channel::addInvited(Client &client)
{
    this->clientsInvited.push_back(client);
}

std::string Channel::getListClients()
{
    std::string str;
    std::vector<Client>::iterator it;
    for (it = this->clients.begin();it < this->clients.end();it++)
    {
        if (it != this->clients.begin())
            str += " ";
        if (this->isFdOperator((it)->sock))
            str += "@";
        str += (it)->nick;
    }
    std::cout << "Clients list of " << this->name << "  : " << str << std::endl;
    return str;
}

bool Channel::isClientOnChannel(Client client)
{
    std::vector<Client>::iterator it;

    for (it = this->clients.begin(); it < this->clients.end(); it++)
        if (client.nick == it->nick)
            return true;
    return false;
}

bool Channel::addClient(Client &client, char *pass)
{
    if (isClientOnChannel(client))
        throw ClientErrMsgException(ERR_USERONCHANNEL(this->srv_hostname, this->name, client.nick), client);
    if (this->isPasswordSet)
        if (this->pass.compare(pass))
            throw ClientErrMsgException(ERR_BADCHANNELKEY(client.nick, this->srv_hostname, this->name), client);

    this->clients.push_back(client);


    // check if client invited to channel and remove from invited list
    // if (std::find(this->clientsInvited.begin(), this->clientsInvited.end(), client) != this->clientsInvited.end())
    printf("\t\tClient %s is invited to channel %s\n", client.nick.c_str(), this->name.c_str());

    eraseInvitedClient(client);

    client.eraseInvitedChannel(this->name);
    
    client._channels.push_back(this);
    return true;
}

void Channel::eraseInvitedClient(Client &client)
{
    std::vector<Client>::iterator it;
    for (it = this->clientsInvited.begin(); it < this->clientsInvited.end(); it++)
    {
        if (it->nick == client.nick)
        {
            this->clientsInvited.erase(it);
            return;
        }
    }
}



void Channel::send_message(Client &client, std::string msg)
{
    (void) client;
    std::vector<Client>::iterator it;
    for (it = this->clients.begin();it < this->clients.end();it++)
        it->send_message(msg);
}

void IRCserv::addNewChannel(std::string name,char *pass, Client &client)
{
    Channel *channel = isChannelExisiting(name);
    if (!channel)
    {
        channel = new Channel(name, pass,client, this->getHostName());
        this->channels.push_back(channel);
        client.send_message(RPL_JOIN(client.nick, client.user, name, client.getIpAddress()));
        client.send_message(RPL_MODEIS(name, this->getHostName(), "+sn"));
        client.send_message(RPL_NAMREPLY(this->getHostName(), channel->getListClients(), channel->getName(), client.nick));
        client.send_message(RPL_ENDOFNAMES(this->getHostName(), client.nick, name));
        // std::cout << "Channel created and added" << std::endl;
    }
    else
    {
        std::cout << "Channel existing" << std::endl;
        //if mode is good
        try
        {
            channel->addClient(client, pass);
            client.send_message(RPL_JOIN(client.nick, client.user, name, client.getIpAddress()));
            client.send_message(RPL_TOPICDISPLAY(this->getHostName(), client.nick, name, channel->getTopic()));
            client.send_message(RPL_TOPICWHOTIME((*channel).getTopicNickSetter(), channel->getTopicTimestamp(),
                client.nick, this->getHostName(), (*channel).getName()));
            client.send_message(RPL_NAMREPLY(this->getHostName(), channel->getListClients(), channel->getName(), client.nick));
            client.send_message(RPL_ENDOFNAMES(this->getHostName(), client.nick, name));
            channel->send_message(client, RPL_JOIN(client.nick, client.user, channel->getName(), client.getIpAddress()));
        }
        catch(const ClientErrMsgException &e)
        {
            e._client.send_message(e.getMessage());
        }
        
    }
}

bool Channel::is_member(Client &client)
{
    std::vector<Client>::iterator it;
    for (it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if (client.nick == it->nick)
            return true;
    }
    return false;
}

void Channel::setInviteOnly(bool value)
{
    isInviteOnlySet = value;
}

void Channel::setKey(const std::string& newKey)
{
    key = newKey;
    isPasswordSet = true;
}
void Channel::setTopic(const std::string& newTopic) 
{
    topic = newTopic;
    isTopicSet = true;
}
void Channel::setUserLimit(int limit) 
{
    userLimit = limit;
}

void Channel::setTopicRestrictions(bool value)
{
    this->isTopicSet = true;
}

void Channel::setOperator(Client &client, bool isOperator)
{
    this->fdOps.push_back(client.sock);
    this->_isOperator = true;
}

void Channel::addOperator(const std::string& nickname)
{
    // bool set = this->isClientOnChannel(*isClientExisiting(nickname));
    // if (set)
    //     this->
}



bool Channel::isInviteOnly()
{
    return isInviteOnlySet;
}

bool Channel::isNickInChannel(std::string nickname)
{
    std::vector<Client>::iterator it;
    for (it = this->clients.begin(); it < this->clients.end(); it++)
    {
        if (it->nick == nickname)
            return true;
    }
    return false;
}