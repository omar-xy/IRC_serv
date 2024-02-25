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

    c_fds[0].fd = this->_sockfd;
    c_fds[0].events = POLLIN;


    
}

void Server::accept_new_connection()
{
    int new_sock = accept(this->_sockfd, (struct sockaddr*)&this->sockaddr, &this->addrlen);
    std::cout << "New connection client_fd : " << new_sock << std::endl;
    this->_cliens_fds.push_back(new_sock);
     for (int i = 1; i <= MAX_CLIENTS; ++i) {
        if (c_fds[i].fd == 0) {
            c_fds[i].fd = new_sock;
            c_fds[i].events = POLLIN;
            break;
        }
    }
    this->clients.insert(std::make_pair(new_sock, new Client(new_sock)));
    this->clients[new_sock]->sendMessage("connected");
}

Client *Server::getClientByIndex(int cIndex)
{
    return clients[c_fds[cIndex].fd];
}

void Server::handle_message(char *msg, int cIndex)
{
    char *cmd;
    Client *client;
    (void)cIndex;

    client = clients[c_fds[cIndex].fd];
    char *tmp = strdup(msg);
    cmd = strtok(tmp, " ");

    if (!strcmp("NICK", cmd))
        client->setName(strtok(NULL, " "));
    if (!strcmp("USER", cmd))
        client->parseUsername(msg);
    if (!strcmp("JOIN", cmd))
        Channel::parseChannelMessage(msg, cIndex);

        
    // std::cout << cmd << std::endl;
}

void Server::receive_message(int cIndex)
{
    char buffer[1024] = { 0 };
    std::cout << "reading fd : "<< c_fds[cIndex].fd << std::endl;
    ssize_t bytes = recv(c_fds[cIndex].fd, buffer, sizeof(buffer), 0);
    // std::cout << "readed "<< bytes << " bytes" << std::endl;
    if (!bytes)
    {
        std::cout << "Client " << c_fds[cIndex].fd << " disconnected" << std::endl;
        close(c_fds[cIndex].fd);
        c_fds[cIndex].fd = 0;
    }
    else
    {
        buffer[bytes] = 0;
        std::cout << "Received : " << buffer << std::endl;
        handle_message(buffer, cIndex);
    }
}

void Server::serve_loop()
{
    while (1)
    {
        if (poll(c_fds, MAX_CLIENTS + 1, 1) < 0)
            throw ApplicationException("Poll Error");
        if (c_fds[0].fd == this->_sockfd && (c_fds[0].revents & POLLIN))
            accept_new_connection();
        for (size_t i = 1; i < MAX_CLIENTS; i++)
        {
            if (c_fds[i].revents & POLLIN)
                receive_message(i);
        }
        
    }
    
}

Server::~Server()
{

}