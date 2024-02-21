#include "Server.hpp"

Server::Server() : _port(5000), _password("pass")
{
}

Server::Server(unsigned int port, std::string password) : _port(port), _password(password)
{
    if (_port > 25000)
        throw ApplicationException("Given port is invalid");
    else if (!_password.length())
        throw ApplicationException("Password isn't set");
    this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) throw ApplicationException("Error while creating socket");

    std::cout << "Sockfd : " << _sockfd << std::endl;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(this->_port);
    sockaddr.sin_family = AF_INET;
    socklen_t addrlen = sizeof(sockaddr);
    if (bind(_sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
        throw ApplicationException("Failing while binding port");
    if (listen(_sockfd, 1024) < 0)
        throw ApplicationException("Failing while listening to port");

    int new_sock;
    new_sock = accept(_sockfd, (struct sockaddr*)&sockaddr, &addrlen);
    std::cout << "new_sock : " << new_sock << std::endl;
}


Server::~Server()
{

}