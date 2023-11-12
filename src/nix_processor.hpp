/// Copyright (c) 2023 Araf Al Jami
#ifndef NIX_PROCESSOR_HPP
#define NIX_PROCESSOR_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <poll.h>
#include "testcases.h"
#include "processor.hpp"

class NixProcessor : public Processor
{
private:    
    pollfd *pfds = nullptr;
    int socket_fd = -1;
    int client_socket_fd = -1;
    const int buffer_size = 4096;
    char* buffer;
public:
    NixProcessor()
    {
        buffer = new char[buffer_size];
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0)
        {
            std::cerr << "Error: socket creation failed" << std::endl;
            exit(-1);
        }

        const int enable = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {
            std::cerr << "Error: setsockopt(SO_REUSEADDR) failed" << std::endl;
            exit(-1);
        }

        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(4244);
        inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

        if (bind(socket_fd, (struct sockaddr *)&hint, sizeof(hint)) < 0)
        {
            std::cerr << "Error: Can't bind to IP/port" << std::endl;
            exit(-1);
        }

        if (listen(socket_fd, SOMAXCONN) < 0)
        {
            std::cerr << "Error: Can't listen !" << std::endl;
            exit(-1);
        }

        pfds = new pollfd;

        pfds->fd = socket_fd;
        pfds->events = POLLIN;
    }
    
    ~NixProcessor()
    {
        std::cout << "Shutting down inpdl listener..." << std::endl;
        if (client_socket_fd != -1)
        {
            close(client_socket_fd);
        }
        if (socket_fd != -1)
        {
            close(socket_fd);
        }
        if (pfds != nullptr)
        {
            delete pfds;
        }
        if (buffer != nullptr)
        {
            delete[] buffer;
        }
    }
    void Process()
    {
        for (;;)
        {
            int poll_count = poll(pfds, 1, -1);

            if (poll_count == -1) {
                perror("poll");
                break;
            }

            if (!(pfds->revents & POLLIN)) 
            {
                continue;
            }
                
            sockaddr_in client;
            socklen_t clientSize = sizeof(client);
            client_socket_fd = accept(socket_fd, (sockaddr *)&client, &clientSize);
            if (client_socket_fd < 0)
            {
                std::cerr << "Error: Problem with client connecting ! " << strerror(errno)  << std::endl;
            }

            bool processed = false;

            memset(buffer, 0, buffer_size);

            int rv = recv(client_socket_fd, buffer, buffer_size, 0);

            if (rv < 0)
            {
                std::cerr << "Error: Problem with recieving ! " << strerror(errno) << std::endl;
                continue;
            }

            for (int i = 0; i < buffer_size && buffer[i]; i++)
            {
                if (buffer[i] == '{')
                {
                    process_test_cases(buffer + i);
                    processed = true;
                    break;
                }
            }

            close(client_socket_fd);

            client_socket_fd = -1;

            if (!watch_mode && processed)
            {
                break;
            }

        }

        close(socket_fd);

        socket_fd = -1;

        delete pfds;

        pfds = nullptr;

        delete[] buffer;

        buffer = nullptr;
    }
};

#endif // NIX_PROCESSOR_HPP