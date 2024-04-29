#include "Bot.hpp"
#include <stdexcept>
#include <sstream>
Bot::Bot(int ip, int port, std::string pass)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw std::runtime_error(":socket() fail!");
    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(sockaddr_in));
    serverAddress.sin_family = AF_INET;
    std::stringstream in_port(pass);
    if (!(in_port >> port) || this->port < 1024 || this->port > 65535)
        throw std::runtime_error("non valid port!");

    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, (const sockaddr *)&serverAddress, sizeof(sockaddr)) < 0)
        throw std::runtime_error('":bind() fail!');

}