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
        
        pthread_t thr_connections;
        pthread_t thr_recv;

    public:
        Server();
        Server(unsigned int port, std::string password);
        ~Server();

        static void *accept_connections(void *obj);
        static void *recv(void *obj);
        void serve_loop();
};
