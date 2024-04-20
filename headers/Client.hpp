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
		std::vector<Channel *> _channels;
		std::vector<std::string> invitedChannels;
		int			registered;
		Client();
		~Client();
		Client(int sock);

		void send_message(std::string msg);
		void send_message(char *msg);
		std::string getIpAddress();

		std::string getFullname();

		std::string getInvitedChannels();

		void disconnect();
		void leaveAllChannels();
		void eraseInvitedChannel(std::string channelName);
		void eraseChannel(std::string channelName);

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
