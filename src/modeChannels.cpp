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


void IRCserv::applyModeFlags(std::string channelName, std::string modeFlags, std::string additionalParams, Client &client)
{
    std::string key= "";
    std::string nickname= "";
    int limit = 0;
    char flag;
    Channel *channel = isChannelExisiting(channelName);
    if (!channel)
    {
        client.send_message(ERR_NOSUCHCHANNEL(this->getHostName(), channelName, client.nick));
        return;
    }
    bool setFlag = true;
    for (int i =0; i < modeFlags.size(); i++)
    {
        flag = modeFlags[i];
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
        switch  (flag)
        {
            case 'i':
                channel->setInviteOnly(setFlag);
                break;
            case 't':
                channel->setTopicRestrictions(setFlag);
                break;
            case 'k':
                if (setFlag)
                {
                    if (additionalParams.empty())
                        client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
                    else
                    {
                        key = additionalParams;
                        channel->setKey(key);
                    }
                }
                else
                    channel->setKey("");
                break;
            case 'o':
                if (additionalParams.empty())
                    client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
                else
                {
                    nickname = additionalParams;
                    if (!channel->is_member(client))
                        client.send_message(ERR_NOSUCHNICK(this->getHostName(), channelName, nickname));
                    else
                        channel->setOperator(client, setFlag);
                }
                break;
            case 'l':
                if (additionalParams.empty())
                    client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
                else
                {
                    limit = atoi(additionalParams.c_str());
                    if (limit && setFlag)
                        channel->setUserLimit(limit);
                    else
                        channel->setUserLimit(0);
                }
                break;
            default :
                client.send_message(ERR_UNKNOWNMODE(client.nick, this->hostname, channelName, flag));
        }
    }
}
