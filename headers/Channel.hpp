#pragma once

#include "header.hpp"

class Client;

class Channel{
    private:
        std::string name;
        std::string pass;
        std::string op;
        int fdOp;
        std::vector<Client*> clients;

        

    public:

        std::string getName();
        std::vector<Client> getClients();

        void setName(std::string name);
        void addClient(Client *c);

        // static void parseChannelMessage(char *msg, Client client);

        Channel(std::string name, char *pass, Client client);
        ~Channel();
};