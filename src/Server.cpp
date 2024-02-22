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
    addrlen = sizeof(sockaddr);
    if (bind(_sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
        throw ApplicationException("Failing while binding port");
    if (listen(_sockfd, 1024) < 0)
        throw ApplicationException("Failing while listening to port");

    memset(c_fds, 0, sizeof(c_fds));

    // Add server socket to poll set
    c_fds[0].fd = this->_sockfd;
    c_fds[0].events = POLLIN;


    // // Creating thread for new connections
    // pthread_create(&this->thr_connections, NULL, &Server::accept_connections, this);
    // while (!_cliens_fds.size())
    //     ;    

    // pthread_create(&this->thr_recv, NULL, &Server::recv, this);
    // pthread_join(this->thr_recv, NULL);

    // Creating thread for handling requests


    
}

void *Server::accept_connections(void *obj)
{
    Server  *self = static_cast<Server *> (obj);
    int new_sock = -1;
    new_sock = accept(self->_sockfd, (struct sockaddr*)&self->sockaddr, &self->addrlen);
    while (new_sock != -1)
    {
        std::cout << "New connection client_fd : " << new_sock << std::endl;
        self->_cliens_fds.push_back(new_sock);
        send(new_sock, "connected", 9, 0);
        new_sock = accept(self->_sockfd, (struct sockaddr*)&self->sockaddr, &self->addrlen);
    }
    std::cout << "Ending listening connections" << std::endl;
    return (obj);
}

void *Server::recv(void *obj)
{
    Server  *self = static_cast<Server *> (obj);
    while (1)
    {
        
        size_t i = 0;
        std::cout << "started looping" << std::endl;
        while (i < self->_cliens_fds.size())
        {
            char buffer[1024] = { 0 };
            std::cout << "checking client : " << self->_cliens_fds.at(i) << std::endl;
            ssize_t bytes = read(self->_cliens_fds.at(i), buffer, 1);
            std::cout << "bytes readed  : " << bytes << std::endl;
            std::cout << "buffer readed : " << buffer << std::endl;
            i++;
        }
    }
}

void Server::serve_loop()
{
    while (1)
    {
        if (poll(c_fds, MAX_CLIENTS + 1, 1) < 0)
            throw ApplicationException("Poll Error");
        
    }
    
}

Server::~Server()
{

}