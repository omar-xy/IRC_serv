#pragma once

#include "header.hpp"

class Channel;

class bot
{
	public:
		std::string		nick;
		int				sock;
		std::string		user;
		std::string		hostname;
		std::string		realname;
		std::string		mode;
		// std::vector<Channel *> _channels;
		// std::vector<std::string> invitedChannels;
		int			registered;
		bot();
		~bot();
		bot(int sock);


		void send_message(std::string msg);
		void send_messageCH(char *msg);
		std::string getIpAddress();
		// std::vector<Channel *> getChannels();

		std::string getInvitedChannels();

		void disconnect();
		void leaveAllChannels();
		void eraseInvitedChannel(std::string channelName);
		void eraseChannel(std::string channelName);

};