#pragma once

#include "header.hpp"

class Client;

class Channel{
    private:

        std::string srv_hostname;
        std::string name;
        std::string pass;
        bool        isPasswordSet;
        bool        isInviteOnlySet;
        std::string op;
        std::vector<int> fdOps;
        std::vector<Client> clients;

        std::string topic;
        std::string topic_usersetter;
        std::string topic_nicksetter;

        int topic_set_timestamp;
        std::string key;
        int userLimit;
        

        bool _isChannelNameValid(std::string name);

    public:
        bool        isTopicSet;
        std::vector<Client> clientsInvited;
        bool isFdOperator(int fd);
        std::string getName();
        std::string getTopicUserSetter();
        std::string getTopicNickSetter();
        std::string getTopicTimestamp();
        std::string getTopic();
        std::vector<Client> getClients();
        std::string getListClients();

        void eraseInvitedClient(Client &client);

        void eraseClient(Client &client);

        bool isClientOnChannel(Client client);
        bool isInviteOnly();
        bool isNickInChannel(std::string nickname);

        void setName(std::string name);
        bool addClient(Client &c, char *pass);
        void send_message(Client &c, std::string message);
        bool is_member(Client &c);
        void setInviteOnly(bool value);
        void setKey(const std::string& newKey);
        void setTopic(const std::string& newTopic);
        void setUserLimit(int limit);
        void setTopicRestrictions(bool value);
        void setOperator(Client &client, bool isOperator);
        void addOperator(const std::string& nickname);

        void addInvited(Client &client);

        // static void parseChannelMessage(char *msg, Client client);

        Channel(std::string name, char *pass, Client &client, std::string srv_hst);
        ~Channel();
};
