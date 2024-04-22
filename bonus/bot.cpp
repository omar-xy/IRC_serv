#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"
#include <iostream>
#include <string>
#include <ctime>
#include <vector>

void helpCommand(Client &client) 
{
    client.send_message("/help - Displays this help message.\n");
    client.send_message("/time - Displays the current time.\n");
    client.send_message("/channels - Lists the channels the bot has joined.\n");
    client.send_message("/funFact - Displays a random fact.\n");
}


void timeCommand(Client &client) {
    time_t now = time(0);
    char* dt = ctime(&now);
    std::string ti(dt);
    client.send_message("The current date and time is: " + ti + "\n");
}


void channelsCommand(const std::vector<Channel *> joinedChannels, Client &client) 
{
    std::cout << "Channels I've joined:" << std::endl;
    for(unsigned int i = 0; i < joinedChannels.size(); i++) 
    {
        client.send_message(joinedChannels[i]->getName());
        client.send_message("\n");
    }
}


void funFactCommand(Client &client) 
{
    client.send_message("Random fact: The first computer bug was an actual bug.");
}

void IRCserv::handleBot(char *msg, Client &client)
{
    char *cmd = strtok(msg, " ");
    if (!cmd || strcmp(cmd, "BOT"))
        return;
    char *chcmd = strtok(NULL, "");
    if (!chcmd)
    {
        client.send_message(ERR_NEEDMOREPARAMS(client.nick, this->getHostName()));
        return;
    }
    std::string command(chcmd);
    if (command == "/help") {
        helpCommand(client);
    } else if (command == "/time") {
        timeCommand(client);
    } else if (command == "/channels") {
        channelsCommand(client.getChannels(), client);
    } else if (command == "/funFact") {
        funFactCommand(client);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }

}


