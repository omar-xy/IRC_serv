#include "include/Client.hpp"

Client::Client(int socket, std::string login, std::string nick) : socket(socket), login(login), nick(nick)
{}