#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"

void IRCserv::handleMode(char *msg, Client &client)
{
    char *tmp;
    tmp = strtok(msg, " ");
    if (strcmp("MODE", tmp))
        return;
    tmp = strtok(NULL, " "); 
    if (!tmp)
    {
        client.send_message("Error: Channel name not provided.");
        return;
    }
    std::string channelName(tmp);
    tmp = strtok(NULL, " ");
    if (!tmp)
    {
        client.send_message("Error: Mode flags not provided.");
        return;
    }
    std::string modeFlags(tmp);
    std::string additionalParams = "";
    tmp = strtok(NULL, "");
    if (tmp)
        additionalParams = tmp;
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
        client.send_message("Error: Channel does not exist.");
        return;
    }
    bool setFlag = true; // Assume we start by setting flags
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
                if(setFlag)
                    channel->setInviteOnly(setFlag);
                else
                    channel->setInviteOnly("");
                break;
            case 't':
                channel->setTopicRestrictions(setFlag);
                break;
            case 'k':
                if (setFlag)
                {
                    key = additionalParams;
                    channel->setKey(key);
                }
                else
                    channel->setKey("");
                break;
            case 'o':
                nickname = additionalParams;
                // channel->is_member(client.)
                // channel->setOperator(client, setFlag);
                break;
            case 'l':
                limit = atoi(additionalParams.c_str());
                if (limit && setFlag)
                    channel->setUserLimit(limit);
                else
                    channel->setUserLimit(0);
                break;
            default :
                client.send_message("Error: Unknown mode flag.");
        }
    }
}
