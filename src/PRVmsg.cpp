#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"


void IRCserv::parsePRIVMSG(char *msg, Client &client)
{
char *tmp;
(void) client;
tmp = strtok(msg, " ");
if (strcmp("PRIVMSG", tmp))
    return;
char *target = strtok(NULL, ":");
if (!target)
{
    client.send_message(":" + this->getHostName() + " 411 " + client.nick + " :No recipient given\r\n");
    return;
}
char *message = strtok(NULL, "");
if (!message)
{
    client.send_message(":" + this->getHostName() + " 412 " + client.nick + " :No text to send\r\n");
    return;
}

bool recipientFound = false;
for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
{
    if (it->second.nick == target) 
    {
        std::string temp = message;
        trim(temp);
		std::string msg_sent = client.nick + ": "+ temp + "\r\n";
        it->second.send_message(msg_sent.c_str());
        recipientFound = true;
        break;
    }
}

if (!recipientFound) {
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if ((*it)->getName() == target) 
        {
            std::string temp_ch = message;
            trim(temp_ch);
		    std::string msg_sent_ch = client.nick + ": "+ temp_ch + "\r\n";
            if ((*it)->is_member(client)) 
                (*it)->send_message(client, msg_sent_ch.c_str());
            else
                client.send_message(":" + this->getHostName() + " 404 " + client.nick + " " + (*it)->getName() + " :Cannot send to channel\r\n");
            recipientFound = true;
            break;
        }
    }
}

if (!recipientFound)
    client.send_message(":" + this->getHostName() + " 401 " + client.nick + " " + target + " :No such nick/channel\r\n");



std::cout << "target : " << target << std::endl;
std::cout << "message : " << message << std::endl;
	
}