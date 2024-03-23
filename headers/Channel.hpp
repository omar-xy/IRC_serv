#pragma once

#include "header.hpp"

class Client;

class Channel{
    private:

        std::string srv_hostname;
        std::string name;
        std::string pass;
        std::string op;
        std::vector<int> fdOps;
        std::vector<Client> clients;

        std::string topic;
        std::string topic_usersetter;
        std::string topic_nicksetter;

        int topic_set_timestamp;

        

    public:
        bool isFdOperator(int fd);
        std::string getName();
        std::string getTopicUserSetter();
        std::string getTopicNickSetter();
        std::string getTopicTimestamp();
        std::string getTopic();
        std::vector<Client> getClients();
        std::string getListClients();

        bool isClientOnChannel(Client &client);

        void setName(std::string name);
        bool addClient(Client &c);
        void send_message(Client &c, std::string message);
        bool is_member(Client &c);

        // static void parseChannelMessage(char *msg, Client client);

        Channel(std::string name, char *pass, Client &client, std::string srv_hst);
        ~Channel();
};