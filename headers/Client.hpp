#pragma once

#include "header.hpp"
class Client
{
	public:
		std::string		nick;
		std::string		user;
		std::string		hostname;
		int			registered;
		int				sock;
		Client();
		Client(int sock);

		void send_message(std::string msg);
		void send_message(char *msg);
		std::string getIpAddress();

		std::string getFullname();


};

// class ClientErrMsgException : public std::exception
// {
//             private:
//                 const std::string _cmessage;
//             public:
//                 ClientErrMsgException() throw();
//                 ClientErrMsgException(Client &bc, std::string msg) throw();
//                 ~ClientErrMsgException() throw();
//                 const Client &_client;
// 				std::string getMessage();
//                 virtual const char* what() const throw(){
//                     return (_cmessage.c_str());
//                 };
// };
