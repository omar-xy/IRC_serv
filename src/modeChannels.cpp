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
    std::string additionalParams = "";
    if (addParams)
        additionalParams = addParams;
    applyModeFlags(channelName, modeFlags, additionalParams, client); 
}

void FInviteOnly(Channel* channel, bool setFlag,  std::string& additionalParams, Client& client, std::string hostName)
{
    channel->setInviteOnly(setFlag);
}

void FKey(Channel* channel, bool setFlag,  std::string& additionalParams, Client& client, std::string hostName)
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


void FUserLimit(Channel* channel, bool setFlag,  std::string& additionalParams, Client &client, std::string hostName)
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
void FOperator(Channel* channel, bool setFlag,  std::string& additionalParams, Client& client, std::string hostName)
{

    if (setFlag)
    {
        if (additionalParams.empty())
            client.send_message(ERR_NEEDMOREPARAMS(client.nick, hostName));
        else
        {
            channel->addOperator(additionalParams, hostName, client);
        }

    }
    else
    {
        if (additionalParams.empty())
            client.send_message(ERR_NEEDMOREPARAMS(client.nick, hostName));
        else
        {
            channel->removeOperator(additionalParams, hostName, client);
        }
    }
}


void FTopicRestrictions(Channel* channel, bool setFlag,  std::string& additionalParams, Client& client, std::string hostName)
{
    channel->isTopicSet = setFlag;
}

void IRCserv::applyModeFlags(std::string channelName, std::string modeFlags, std::string additionalParams, Client &client)
{
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
    std::map<char, void (*)(Channel*, bool,  std::string&, Client&, std::string)> modeActions;
    modeActions['i'] = &FInviteOnly;
    modeActions['t'] = &FTopicRestrictions;
    modeActions['k'] = &FKey;
    modeActions['o'] = &FOperator;
    modeActions['l'] = &FUserLimit;

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

        std::map<char, void (*)(Channel*,  bool, std::string&, Client&, std::string)>::iterator actionIt = modeActions.find(flag);
        if (actionIt != modeActions.end())
        {
            actionIt->second(channel, setFlag, additionalParams, client, this->getHostName());
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
    std::cout << channel->getKey() << std::endl;
}
//     std::string key= "";
//     std::string nickname= "";
//         std::string mode = "";
//     int limit = 0;
//     char flag;
//     Channel *channel = isChannelExisiting(channelName);
//     if (!channel)
//     {
//         client.send_message(ERR_NOSUCHCHANNEL(this->getHostName(), channelName, client.nick));
//         return;
//     }
//     bool setFlag = true;
//     for (std::string::const_iterator it = modeFlags.begin(); it != modeFlags.end(); ++it)
//     {
//         flag = *it;
//         if (flag == '+')
//         {
//             setFlag = true;
//             continue;
//         }
//         else if (flag == '-')
//         {
//             setFlag = false;
//             continue;
//         }
//         switch  (flag)
//         {
//             case 'i':
//                 channel->setInviteOnly(setFlag);
//                 break;
//             case 't':
//                 channel->setTopicRestrictions(setFlag);
//                 break;
//             case 'k':
//                 if (setFlag)
//                 {
                
//                     if (additionalParams.empty())
//                         client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
//                     else
//                     {
//                         key = additionalParams;
//                         channel->setKey(key);
//                     }
//                 }
//                 else
//                     channel->setKey("");
//                 break;
//             case 'o':
//                 if (additionalParams.empty())
//                     client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
//                 else
//                 {
//                     nickname = additionalParams;
//                     if (!channel->is_member(client))
//                         client.send_message(ERR_NOSUCHNICK(this->getHostName(), channelName, nickname));
//                     else
//                         channel->setOperator(client, setFlag);
//                 }
//                 break;
//             case 'l':
//                 if (additionalParams.empty())
//                     client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
//                 else
//                 {
//                     limit = atoi(additionalParams.c_str());
//                     if (limit && setFlag)
//                         channel->setUserLimit(limit);
//                     else
//                         channel->setUserLimit(0);
//                 }
//                 break;
//             default :
//                 client.send_message(ERR_UNKNOWNMODE(client.nick, this->hostname, channelName, flag));
        
//         }
//     }
//     std::cout << "MODE " << channelName << " " << channel->getMode() << " " << additionalParams << std::endl;