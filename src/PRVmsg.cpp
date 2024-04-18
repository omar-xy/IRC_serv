#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"


void IRCserv::parsePRIVMSG(char *msg, Client &client)
{  
    char *prefix = strtok(msg, " ");
    char *target = strtok(NULL, " ");
    char *text = strtok(NULL, "");
    if  (!prefix || strcmp(prefix, "PRIVMSG"))
        return ;
    if (text && text[0] == ':')
        text++;
    else if (text && text[0] != ':')
		text = strtok(text, " ");
    if (!target || !text)
    {
        client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
        return ;
    }
    if (target[0] == '#')
    {
        Channel *ch = isChannelExisiting(target);
        if (ch)
        {
            if (ch->is_member(client))
                ch->send_message(client, text);
            else
                client.send_message(ERR_CANNOTSENDTOCHAN(client.nick, target, this->getHostName()));
        }
        else
            client.send_message(ERR_NOSUCHCHANNEL(this->getHostName(), target, client.nick));
    }
    else
    {
        Client *receiver = isClientExisiting(target);
        if (receiver)
            receiver->send_message(PRIVMSG_FORMAT(client.nick, client.user, client.hostname, target, text));
        else
            client.send_message(ERR_NOSUCHNICK(this->getHostName(), target, client.nick));
    }
}