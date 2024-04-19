#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"
#include <cstdlib>

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


void IRCserv::applyModeFlags(std::string channelName, std::string modeFlags, std::string additionalParams, Client &client)
{
    Channel *channel = isChannelExisiting(channelName);
    if (!channel)
    {
        client.send_message(ERR_NOSUCHCHANNEL(this->getHostName(), channelName, client.nick));
        return;
    }

    bool setFlag = true;
    std::map<char, void (*)(Channel*, bool, const std::string&, Client&)> modeActions;
    modeActions['i'] = &setInviteOnly;
    modeActions['t'] = &setTopicRestrictions;
    modeActions['k'] = &setKey;
    modeActions['o'] = &setOperator;
    modeActions['l'] = &setUserLimit;

    for (std::string::const_iterator it = modeFlags.begin(); it != modeFlags.end(); ++it)
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

        std::map<char, void (*)(Channel*, bool, const std::string&, Client&)>::iterator actionIt = modeActions.find(flag);
        if (actionIt != modeActions.end())
        {
            actionIt->second(channel, setFlag, additionalParams, client);
        }
        else
        {
            client.send_message(ERR_UNKNOWNMODE(client.nick, this->hostname, channelName, flag));
        }
    }

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
//         if (setFlag)
//             mode += "+";
//         else
//             mode += "-";
//         mode += flag;
//         channel->setMode(mode);
//     }
//     std::cout << "MODE " << channelName << " " << channel->getMode() << " " << additionalParams << std::endl;