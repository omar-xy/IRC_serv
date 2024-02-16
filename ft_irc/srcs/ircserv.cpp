/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otaraki <otaraki@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 16:16:28 by otaraki           #+#    #+#             */
/*   Updated: 2024/02/16 19:16:16 by otaraki          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ircserv.hpp"


IRCServer::IRCServer()
{
    this->port = 0;
    this->sock = 0;
    this->kq = 0;
    this->pswd = "password";
}


IRCServer::IRCServer(unsigned int _port,  int _sock, int _kq, std::string _pswd)
{
    this->port = _port;
    this->sock = _sock;
    this->kq = _kq;
    this->pswd = _pswd;
}

IRCServer::~IRCServer()
{
    std::cout << "Server is shutting down" << std::endl;
}

// std::string IRCServer::toString()
// {
//     return "Server: " + this->server + "\nPort: " + this->port + "\nPassword: " + this->pswd + "\nNick: " + this->nick + "\nUser: " + this->user + "\nRealname: " + this->realname;
// }


//setters and getters

void IRCServer::setPort(unsigned int port)
{
    this->port = port;
}


void IRCServer::setSock( int sock)
{
    this->sock = sock;
}


void IRCServer::setPswd(std::string pswd)
{
    this->pswd = pswd;
}


unsigned int IRCServer::getPort()
{
    return this->port;
}

 int IRCServer::getSock()
{
    return this->sock;
}

std::string IRCServer::getPswd()
{
    return this->pswd;
}


//member functions create socket, connect, run_event_loop, disconnect, run
void IRCServer::CreateScoket()
{
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock == -1)
    {
        perror("socket");
        return;
    }
    
    // in thr code below, we are creating a socket and binding it to the port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(this->sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        close(this->sock );
        return;
    }
    
    // in the code below, we are allowing more than one connection
    if (listen(this->sock , 5) == -1)
    {
        perror("listen");
        close(this->sock );
        return;
    }
    // in the code below, we are creating a vector of pollfd and adding the socket to it
   // this->fds.push_back((pollfd){this->sock , POLLIN, 0});//POLLIN is a flag that indicates that data other than high-priority data may be read without blocking
}

void IRCServer::connect()
{
    this->kq = kqueue();
    if (this->kq == -1)
    {
        perror("kqueue");
        return;
    }
    struct kevent evSet;
    EV_SET(&evSet, this->sock, EVFILT_READ, EV_ADD, 0, 0, 0);
    kevent(kq, &evSet, 1, NULL, 0, NULL);
}

void    IRCServer::run_event_loop()
{
    struct kevent evSet;
    struct kevent evList[MAX_EVENTS];
    struct sockaddr_storage addr;
    socklen_t socklen = sizeof(addr);

    while(1)
    {
        int num_events = kevent(this->kq, NULL, 0, evList, MAX_EVENTS, NULL);
        if (num_events < 1)
        {
            perror("kevent");
            return;
        }
        for (int i =0; i < num_events; i++)
        {
            if (evList[i].ident == this->sock)
            {
                int client = accept(this->sock, (struct sockaddr *)&addr, &socklen);
                if (client == -1)
                {
                    perror("accept");
                    return;
                }
                int flags = fcntl(client, F_GETFL, 0);
                fcntl(client, F_SETFL, flags | O_NONBLOCK);
                EV_SET(&evSet, client, EVFILT_READ, EV_ADD, 0, 0, 0);
                kevent(this->kq, &evSet, 1, NULL, 0, NULL);
                send_welcome_msg(client);
            }
            else
            {
                recv_msg(evList[i].ident);
            }
        }
    }
}


void IRCServer::run()
{
    CreateScoket();
    connect();
    run_event_loop();
    disconnect();
}


void IRCServer::disconnect()
{
    close(this->sock);
    close(this->kq);
    std::cout << "Server is shutting down" << std::endl;
}

int IRCServer::get_connection(int fds)
{
    for (size_t i = 0; i < this->fds.size(); i++)
    {
        if (this->fds[i].fd == -1)
            return i;
    }
    return -1;
}

int IRCServer::conn_add(int fds)
{
    if(fds < 1)
    {
        return -1;
    }
    int i = get_connection(0);
    if (i == -1)
        return -1;
    this->fds[i].fd = fds;
    return 0;
}

int IRCServer::conn_del(int fds)
{
    if (fds < 1)
        return -1;
    int i = get_connection(fds);
    if (i == -1)
        return -1;
    this->fds[i].fd = 0;
    return close(fds);
}

void IRCServer::recv_msg(int s) 
{
    std::vector<char> buf(MAX_MSG_SIZE);
    int bytes_read = recv(s, buf.data(), buf.size() - 1, 0);
    if (bytes_read == -1) {
        perror("recv failed");
        return;
    }
    if (bytes_read == 0) {
        std::cout << "client #" << get_connection(s) << " disconnected" << std::endl;
        conn_del(s);
        exit(0);
    }
    buf[bytes_read] = '\0';
    std::cout << "client #" << get_connection(s) << ": " << buf.data() << std::endl;
    std::cout.flush();
}

void IRCServer::send_welcome_msg(int s) 
{
    std::string msg = "welcome! you are client #" + std::to_string(get_connection(s)) + "!\n";
    if (send(s, msg.c_str(), msg.size(), 0) == -1) {
        perror("send failed");
    }
}



