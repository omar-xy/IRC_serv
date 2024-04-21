#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"

void IRCserv::handleMode(char *msg, Client &client)
{
    char *pref;
    pref = strtok(msg, " ");
    char *target = strtok(NULL, " ");
    char *flag = strtok(NULL, " ");
    char *addParams = strtok(NULL, "");
    if (!pref || strcmp("MODE", pref))
        return;
    if (!flag || !target)
    {
        client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
        return;
    }
    std::string channelName(target);
    std::string modeFlags(flag);
    applyModeFlags(channelName, modeFlags, addParams, client); 
}

void FInviteOnly(Channel* channel, bool setFlag, const std::string& additionalParams, Client& client, std::string hostName)
{
    channel->setInviteOnly(setFlag);
}

void FKey(Channel* channel, bool setFlag, const std::string& additionalParams, Client& client, std::string hostName)
{
    if (setFlag)
    {
    
        if (additionalParams.empty())
            client.send_message(ERR_NEEDMOREPARAMS(client.nick, hostName));
        else
        {
            channel->setKey(additionalParams);
            channel->isPasswordSet = true;
        }
    }
    else
        channel->isPasswordSet = false;
}


void FUserLimit(Channel* channel, bool setFlag, const std::string& additionalParams, Client &client, std::string hostName)
{
    int limit = 0;
    if (setFlag)
    {
        if (additionalParams.empty())
            client.send_message(ERR_NEEDMOREPARAMS(client.nick, hostName));
        else
        {
            limit = atoi(additionalParams.c_str());
            if (limit)
                channel->setUserLimit(limit);
            else
                channel->setUserLimit(0);
        }
    }
    else
       channel->setUserLimit(0);
}

void FOperator(Channel* channel, bool setFlag, const std::string& additionalParams, Client& client, std::string hostName)
{
    if (setFlag)
        channel->addOperator(additionalParams, hostName, client);
    else
        channel->removeOperator(additionalParams, hostName, client);
}

void FTopicRestrictions(Channel* channel, bool setFlag, const std::string& additionalParams, Client& client, std::string hostName)
{
    channel->isTopicSet = setFlag;
}

void IRCserv::applyModeFlags(std::string channelName, std::string modeFlags, char *addParams, Client &client)
{
    std::vector<std::string> splitParams = split(addParams, ' ');
    std::string additionalParams = "";
    std::string mode = "";
    Channel *channel = isChannelExisiting(channelName);
    if (!channel)
    {
        client.send_message(ERR_NOSUCHCHANNEL(this->getHostName(), channelName, client.nick));
        return;
    }
    if (channel->isFdOperator(client.sock) == false)
    {
        client.send_message(ERR_CHANOPRIVSNEEDED(client.nick, this->getHostName(), channel->getName()));
        return ;
    }
    bool setFlag = true;
    std::map<char, void (*)(Channel*, bool, const std::string&, Client&, std::string)> modeActions;
    modeActions['i'] = &FInviteOnly;
    modeActions['t'] = &FTopicRestrictions;
    modeActions['k'] = &FKey;
    modeActions['o'] = &FOperator;
    modeActions['l'] = &FUserLimit;

    size_t i = 0;
    for (std::string::iterator it = modeFlags.begin(); it != modeFlags.end(); ++it)
    {
        char flag = *it;
        if (flag == '+')
        {
            setFlag = true;
            continue;
        }
        else if (flag == '-')
        {
            setFlag = false;
            continue;
        }

        std::map<char, void (*)(Channel*,  bool, const std::string&, Client&, std::string)>::iterator actionIt = modeActions.find(flag);
        if (actionIt != modeActions.end())
        {
            if (splitParams[i].empty())
            {
                client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
                return;
            }
            actionIt->second(channel, setFlag, splitParams[i], client, this->getHostName());
            i++;
            if (setFlag)
                mode += "+";
            else
                mode += "-";
            mode += flag;
            channel->setMode(mode);
        }
        else
        {
            client.send_message(ERR_UNKNOWNMODE(client.nick, this->hostname, channelName, flag));
        }
    }
}
