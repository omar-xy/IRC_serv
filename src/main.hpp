#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <pthread.h>

#define MAX_CLIENTS 64

#include "Client.hpp"
#include "Server.hpp"


std::vector<std::string> split(const std::string &s, char delimiter);

class ApplicationException : public std::exception {
    private:
        const std::string _message;

    public:
        ApplicationException(std::string msg) throw(): _message(msg) {};
        ApplicationException() throw() {};
        ~ApplicationException() throw() {};
        virtual const char* what() const throw() {
            return _message.c_str();
        }
};

