#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <poll.h>

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    unsigned int port = std::stoi(argv[1]);
    std::string passwd = argv[2];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        close(sock);
        return 1;
    }

    if (listen(sock, 1) == -1)
    {
        perror("listen");
        close(sock);
        return 1;
    }

    int client = accept(sock, NULL, NULL);
    if (client == -1)
    {
        perror("accept");
        close(sock);
        return 1;
    }

    char buff[1024];
    ssize_t len = recv(client, buff, sizeof(buff) - 1, 0);
    if (len == -1)
    {
        perror("recv");
        close(client);
        close(sock);
        return 1;
    }

    buff[len] = '\0';
	std::string password = buff;
	password = password.substr(5, password.size() - 7); 
	std::cout << "Received: |" << password << "| size: " << password.size() << std::endl;
	std::cout << "Expected: |" << passwd << "| size: " << passwd.size() << std::endl;
	if (password != passwd)
	{
		std::cerr << "Invalid password" << std::endl;
		close(client);
		close(sock);
		return 1;
	}
    // Create poll
    struct pollfd fds[1];
    fds[0].fd = client;
    fds[0].events = POLLIN;

    while (true)
    {
        int ret = poll(fds, 1, -1);
        if (ret == -1)
        {
            perror("poll");
            close(client);
            close(sock);
            return 1;
        }
        if (fds[0].revents & POLLIN)
        {
            ssize_t len = recv(client, buff, sizeof(buff) - 1, 0);
            if (len == -1)
            {
                perror("recv");
                close(client);
                close(sock);
                return 1;
            }
            if (len == 0)
            {
                std::cout << "Connection closed" << std::endl;
                break;
            }
            buff[len] = '\0';
            std::cout << "Received: " << buff << std::endl;
        }
    }

	close(client);
	close(sock);


    return 0;
}
