#pragma once

#include "header.hpp"
#include "IRCserv.hpp"
#include "replies.hpp"
#include <cstdlib>

class Client;
class IRCserv;

class Channel{
   

        std::string srv_hostname;
        std::string name;
        std::string pass;
        std::string op;
        std::vector<Client> clients;

        std::string topic;
        std::string topic_usersetter;
        std::string topic_nicksetter;

        int topic_set_timestamp;
        std::string key;
        int userLimit;
        std::string mode;

        IRCserv *serv;

        bool _isChannelNameValid(std::string name);

    public:
        std::vector<int> fdOps;
        bool        isPasswordSet;
        bool        isInviteOnlySet;
        bool        _isOperator;
        bool        isTopicSet;
        std::vector<Client> clientsInvited;
        std::string getName();
        std::string getTopicNickSetter();
        std::string getTopicUserSetter();
        std::string getTopicTimestamp();
        std::string getTopic();
        std::string getKey();
        std::string getMode();
        std::string getOp();
        int         getuserLimit();

        std::vector<Client> getClients();
        std::string getListClients();

        void eraseInvitedClient(Client &client);

        void eraseClient(Client &client);
        void eraseChannel(std::string channelName);
        void eraseOp(int fd);



        bool isFdOperator(int fd);
        bool isClientOnChannel(Client client);
        bool isInviteOnly();
        bool isNickInChannel(std::string nickname);
        bool isKeySet();
        bool isUserLimitSet();
        bool isTopicRestrictionsSet();
        bool isOperator(std::string nickname);
        bool isModeSet(std::string mode);



        void setName(std::string name);
        void setTopic(std::string topic);
        
        
        bool addClient(Client &c, char *pass);
        bool partClient(Client &c, std::string reason);
        bool quitClient(Client &c, std::string reason);
        bool is_member(Client &c);


        void send_message(Client &c, std::string message);
        void send_message(std::string message);
        
        void setMode(const std::string& newMode);
        void setKey(std::string key);
        void setUserLimit(int limit);
        void setTopicRestrictions(bool setFlag);

        void setOperator(Client &client, bool setFlag);
        void addOperator(const std::string& nickname, std::string hostname, Client &client);
        void removeOperator(const std::string& nickname, std::string hostName, Client &client);

        void addInvited(Client &client);
        void setInviteOnly(bool setFlag);
        bool isClientInvited(Client &client);
        // Client& getClientByNick(std::string nickname);

        // static void parseChannelMessage(char *msg, Client client);

        Channel(std::string name, char *pass, Client &client, IRCserv *serv);
        ~Channel();
};
