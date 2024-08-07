#pragma once

#include "header.hpp"

class Channel;

class Client
{
	public:
		std::string		nick;
		int				sock;
		std::string		user;
		std::string		hostname;
		std::string		realname;
		std::string		mode;
		std::string		line; // PASS xxx/r/nNICK 
		std::vector<Channel *> _channels;
		std::vector<std::string> invitedChannels;
		int			registered;
		Client();
		~Client();
		Client(int sock);


		void send_message(std::string msg);
		void send_messageCH(char *msg);
		std::string getIpAddress();
		std::vector<Channel *> getChannels();

		std::string getFullname();

		std::string getInvitedChannels();

		void disconnect();
		void leaveAllChannels(std::string reason);
		void eraseInvitedChannel(std::string channelName);
		void eraseChannel(std::string channelName);

		bool operator == (const Client &c);
		bool operator != (const Client &c);


};

class ClientErrMsgException : public std::exception
{
    private:
        const std::string _cmessage;
        ClientErrMsgException() throw();
    public:
        ClientErrMsgException(std::string msg, Client &bc) throw();
        virtual ~ClientErrMsgException() throw();
        Client &_client;
		const std::string getMessage() const;
        virtual const char* what() const throw()
		{
            return (_cmessage.c_str());
        };
};
