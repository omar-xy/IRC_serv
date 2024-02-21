#pragma once

#include "main.hpp"

class Server {
    private:
        const unsigned int _port;
        const std::string _password;

        struct sockaddr_in sockaddr;
        int _sockfd;

    public:
        Server();
        Server(unsigned int port, std::string password);
        ~Server();


};
