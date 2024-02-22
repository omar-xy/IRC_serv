#pragma once

#include "main.hpp"

class Server {
    private:
        const unsigned int _port;
        const std::string _password;

        struct sockaddr_in sockaddr;
        socklen_t addrlen;
        int _sockfd;
        std::vector<int> _cliens_fds;
        
        struct pollfd c_fds[MAX_CLIENTS + 1];
        

    public:
        Server();
        Server(unsigned int port, std::string password);
        ~Server();

        void accept_new_connection();
        void receive_message(int cIndex);
        void serve_loop();
};
