#include "../headers/IRCserv.hpp"
#include "../headers/replies.hpp"
#include <iostream>
#include <string>
#include <ctime>
#include <vector>

void helpCommand() 
{
    std::cout << "/help - Displays this help message." << std::endl;
    std::cout << "/time - Displays the current time." << std::endl;
    std::cout << "/channels - Lists the channels the bot has joined." << std::endl;
    std::cout << "/funFact - Displays a random fact." << std::endl;
}


void timeCommand() {
    time_t now = time(0);
    char* dt = ctime(&now);
    std::cout << "The current date and time is: " << dt << std::endl;
}


void channelsCommand(const std::vector<Channel *> joinedChannels) 
{
    std::cout << "Channels I've joined:" << std::endl;
    for(unsigned int i = 0; i < joinedChannels.size(); i++) {
        std::cout << joinedChannels[i]->getName() << std::endl;
    }
}


void funFactCommand() {
    std::cout << "Random fact: The first computer bug was an actual bug." << std::endl;
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
        helpCommand();
    } else if (command == "/time") {
        timeCommand();
    } else if (command == "/channels") {
        channelsCommand(client.getChannels());
    } else if (command == "/funFact") {
        funFactCommand();
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }

}


