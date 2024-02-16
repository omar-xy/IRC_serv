#ifndef IRC_HPP
#define IRC_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <poll.h>
#include <vector>
#include <sys/event.h>
#include <sys/time.h>

#define NUM_CLIENTS 10
#define MAX_EVENTS 32
#define MAX_MSG_SIZE 256

class IRCServer
{
    private:
        // std::string server;
        unsigned int  port;
        int sock;
        int kq;
        std::string pswd;
        // std::string nick;
        // std::string user;
        // std::string realname;
        std::vector<pollfd> fds;
        // std::vector<std::string> channels;
        // std::vector<std::string> users;
        // std::vector<std::string> messages;
        // std::vector<IRCChannel> channels;
        // std::vector<IRCUser> users;
    public:
        IRCServer();
        IRCServer(unsigned int _port, int _sock, int _kq, std::string _pswd);
        ~IRCServer();
        // std::string toString();
 
        //setters and getters
        void setPort(unsigned int port);
        void setPswd(std::string pswd);
        void setSock(int sock);
        // void setServer(std::string server);
        // void setNick(std::string nick);
        // void setUser(std::string user);
        // void setRealname(std::string realname);

        unsigned int getPort();
        int getSock();
        std::string getPswd();
        // std::string getServer();
        // std::string getNick();
        // std::string getUser();
        // std::string getRealname();

        void CreateScoket();
        void connect();
        void run_event_loop();
        void disconnect();
        void run();// main 


        int get_connection(int fds);
        int conn_add(int fds);
        int conn_del(int fds);
        void recv_msg(int s);
        void send_welcome_msg(int s);



        // std::vector<IRCChannel> getChannels();
        // std::vector<IRCUser> getUsers();
        // void setName(std::string name);
        // void setChannels(std::vector<IRCChannel> channels);
        // void setUsers(std::vector<IRCUser> users);
};

#endif