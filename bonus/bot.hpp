#pragma once

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Bot : public std::exception {
    private:
        int fd;
        int port;
    public:
        Bot(int ip, int port, std::string pass);
};