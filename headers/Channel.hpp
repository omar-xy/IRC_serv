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

        std::string topic;
        std::string topic_usersetter;
        std::string topic_nicksetter;
        int topic_set_timestamp;

        

    public:

        std::string getName();
        std::string getTopicUserSetter();
        std::string getTopicNickSetter();
        std::string getTopicTimestamp();
        std::string getTopic();
        std::vector<Client*> getClients();

        void setName(std::string name);
        void addClient(Client *c);

        // static void parseChannelMessage(char *msg, Client client);

        Channel(std::string name, char *pass, Client client);
        ~Channel();
};