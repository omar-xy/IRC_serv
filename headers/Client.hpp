#pragma once

#include "header.hpp"

class Channel;

class Client
{
	public:
		std::string		nick;
		std::string		user;
		std::string		hostname;
		std::vector<Channel *> _channels;
		int			registered;
		int				sock;
		Client();
		~Client();
		Client(int sock);

		void send_message(std::string msg);
		void send_message(char *msg);
		std::string getIpAddress();

		std::string getFullname();

		void disconnect();
		void leaveAllChannels();


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
                virtual const char* what() const throw(){
                    return (_cmessage.c_str());
                };
};
