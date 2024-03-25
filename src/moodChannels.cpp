#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"


void IRCserv::handleMood(char *msg, Client &client)
{
    char *tmp;
    tmp = strtok(msg, " ");
    if (strcmp("MOOD", tmp))
        return;
    // "MOOD #channel +i-k-i+k"
    tmp = strtok(NULL, " "); // This should be the channel name
    if (!tmp)
    {
        // client.send_message("Error: Channel name not provided.");
        return;
    }
    std::string channelName(tmp);
    tmp = strtok(NULL, " ");
    if (!tmp) 
    {
        // client.send_message("Error: Mode changes not provided.");
        return;
    }
    std::map<std::string, Channel> channels;
    std::string modeChanges(tmp);
    std::vector<char> flags;
    // i need to store the flags in a vector
    for (size_t i = 0; i < modeChanges.size(); i++) 
    {
        if (modeChanges[i] == '+' || modeChanges[i] == '-') 
        {
            flags.push_back(modeChanges[i]);
        }
    }
    
    // for (size_t i = 0; i < flags.size(); i += 2) 
    // {
    //     char flag = flags[i];
    //     char mode = flags[i + 1];
    //     if (mode == 'i') 
    //     {
    //         if (flag == '+') 
    //         {
    //             channels[channelName].setInviteOnly(true);
    //             client.send_message("Mode set to invite-only.");
    //         } 
    //         else if (flag == '-') 
    //         {
    //             channels[channelName].setInviteOnly(false);
    //             client.send_message("Mode unset from invite-only.");
    //         }
    //     } 
    //     else if (mode == 'k')
    //     {
    //         // Similar logic for handling key changes
    //     }
}

