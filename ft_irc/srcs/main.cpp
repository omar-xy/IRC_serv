#include "../incs/ircserv.hpp"


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << "<pswd>" << std::endl;
        return 1;
    }

    IRCServer irc(atoi(argv[1]), 0, 0, argv[2]);
    irc.run();
    return 0;
}


