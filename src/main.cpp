#include "main.hpp"


void handle_quit(int c)
{
    (void)c;
    throw ApplicationException("Server quitting");
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    try
    {
        signal(SIGTERM, handle_quit);
        Server serv(atoi(argv[1]),argv[2]);
        serv.serve_loop();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: Caught exception:" << '\n';
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }
    return 0;
}
