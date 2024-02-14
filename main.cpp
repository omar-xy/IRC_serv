#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <poll.h>
#include <vector>

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

    if (listen(sock, 5) == -1) // Allow more than one connection
    {
        perror("listen");
        close(sock);
        return 1;
    }

    std::vector<pollfd> fds;
    fds.push_back((pollfd){sock, POLLIN});

    while (1)
    {
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret == -1)
        {
            perror("poll");
            close(sock);
            return 1;
        }

        if (fds[0].revents & POLLIN)
        {
            int client = accept(sock, NULL, NULL);
            if (client == -1)
            {
                perror("accept");
                close(sock);
                return 1;
            }

            char buff[2048]; // Increase buffer size
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
            password = password.substr(5, passwd.size());
            std::cout << "Received: |" << password << "| size: " << password.size() << std::endl;
            std::cout << "Expected: |" << passwd << "| size: " << passwd.size() << std::endl;
            if (password != passwd)
            {
                std::cerr << "Invalid password" << std::endl;
                close(client);
            }
            else
            {
                fds.push_back((pollfd){client, POLLIN});
            }
        }
        for (size_t i = 1; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                char buff[2048];
                ssize_t len = recv(fds[i].fd, buff, sizeof(buff) - 1, 0);
                if (len == -1)
                {
                    perror("recv");
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                }
                else if (len == 0)
                {
                    std::cout << "Connection closed" << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                }
                else
                {
                    buff[len] = '\0';
                    std::cout << "Received from client " << i << ": " << buff << std::endl;
                }
            }
        }
    }
    
    close(sock);
    return 0;
}
