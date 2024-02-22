#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <pthread.h>
#include "Server.hpp"

#define MAX_CLIENTS 64

class ApplicationException : public std::exception {
    private:
        const char * _message;

    public:
        ApplicationException(std::string msg) throw(): _message(msg.c_str()) {};
        ApplicationException() throw() {};
        ~ApplicationException() throw() {};
        virtual const char* what() const throw() {
            return _message;
        }
};

