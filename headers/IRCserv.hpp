#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>

#include "replies.hpp"
#include "Client.hpp"
#include <sys/poll.h>
#include <iterator>

std::vector<std::string> split(const std::string &s, char delimiter);

class	Client;
class	Channel;

#define SERVER_PREFIX "IRC"
#define SERVER_SUFFIX "localhost"
#define SERVER_HOSTNAME SERVER_PREFIX + SERVER_SUFFIX

class IRCserv {
	private:
		std::string			password;
		int					port;
		int					sock;
		std::vector<pollfd>	fds;
		std::map<int, Client>	clients;
		std::string			hostname;

		std::vector<Channel*> channels;

		void handle_message(char *msg, Client &client);
        void parseChannelMessage(char *msg, Client &client);
		void parsePartMessage(char *msg, Client &client);
		void parsePRIVMSG(char *msg, Client &client);
		void handleMode(char *msg, Client &client);

		// --- INVITE ---
		void handleInvite(char *msg, Client &client);

		// --- KICK ---
		void handleKick(char *msg, Client &client);

		// --- TOPIC ---
		void handleTopic(char *msg, Client &client);

		std::string getHostName();
		Channel *isChannelExisiting(std::string name);
		void addNewChannel(std::string name, char *pass, Client &client);
		void applyModeFlags(std::string channelName, std::string modeFlags, std::string additionalParams, Client &client);
		void	inviteClinetToChannel(Client &invitedClient, Channel &channel, Client &client);
		bool isValidNick(std::string nick);
		std::string removeTail(std::string buff);


	public:
		IRCserv(std::string port, std::string password);
		Client *isClientExisiting(std::string name);
		void	init();
		void	debug(std::string msg, int status);
		void	loop();
		void 	addClient();
		void	registeredAction(Client &client, std::string &buff);
};


void       trim(std::string& str);