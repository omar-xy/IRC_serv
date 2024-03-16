#pragma once

#include "main.hpp"

class Client;

class Channel{
    private:
        std::string name;
        std::string op;
        std::vector<Client*> clients;

        

    public:

        std::string getName();
        std::vector<Client> getClients();

        void setName(std::string name);
        void addClient(Client *c);

        static void parseChannelMessage(char *msg, int cIndex);

        Channel(std::string name, std::string pass, int opFd);
        ~Channel();
};