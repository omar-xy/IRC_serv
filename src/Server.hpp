#pragma once

#include "main.hpp"
#include "Channel.hpp"
class Client;
class Server {
    private:
        const unsigned int _port;
        const std::string _password;

        struct sockaddr_in sockaddr;
        socklen_t addrlen;
        int _sockfd;

        struct pollfd c_fds[MAX_CLIENTS + 1];
        
        std::vector<int> _cliens_fds;
        // std::vector<Channel*> channels;
        std::map<int, Client*> clients;
        
        
        Client *getClientByIndex(int cIndex);

    public:
        Server();
        Server(unsigned int port, std::string password);
        ~Server();


        void accept_new_connection();
        void receive_message(int cIndex);
        void serve_loop();
        void handle_message(char *msg, int cIndex);
};
