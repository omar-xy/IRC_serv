#pragma once

#include "main.hpp"

class Client {
    private:
        int _sockfd;
        // struct sockaddr_in sockaddr;
        // socklen_t addrlen;

        std::string name;
        std::string username;
        std::string realname;
        
        

    public:
        Client();
        Client(int fd);
        ~Client();

        void setName(std::string name);
        void setUser(std::string user);

        void parseUsername(char *message);

        std::string getName();
        std::string getUser();

        void sendMessage(std::string msg);

        bool is_registred();

};
