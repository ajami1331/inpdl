/// Copyright (c) 2023 Araf Al Jami
#ifndef NIX_PROCESSOR_HPP
#define NIX_PROCESSOR_HPP

#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include "testcases.h"

struct pollfd *pfds = NULL;
int socket_fd = -1;
int client_socket_fd = -1;
const int buffer_size = 4096;
char *buffer;

void nix_processor_init()
{
    buffer = malloc(sizeof(char) * buffer_size);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        fprintf(stderr, "Error: socket creation failed: %s\n", strerror(errno));
        exit(-1);
    }

    const int enable = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        fprintf(stderr, "Error: setsockopt(SO_REUSEADDR) failed: %s\n", strerror(errno));
        exit(-1);
    }

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(4244);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(socket_fd, (struct sockaddr *)&hint, sizeof(hint)) < 0)
    {
        fprintf(stderr, "Error: bind failed: %s\n", strerror(errno));
        exit(-1);
    }

    if (listen(socket_fd, SOMAXCONN) < 0)
    {
        fprintf(stderr, "Error: listen failed: %s\n", strerror(errno));
        exit(-1);
    }

    pfds = malloc(sizeof(struct pollfd));

    pfds->fd = socket_fd;
    pfds->events = POLLIN;
}

void nix_processor_shutdown()
{
    fprintf(stdout, "Shutting down inpdl listener...\n");
    if (client_socket_fd != -1)
    {
        close(client_socket_fd);
    }
    if (socket_fd != -1)
    {
        close(socket_fd);
    }
    if (pfds != NULL)
    {
        free(pfds);
    }
    if (buffer != NULL)
    {
        free(buffer);
    }
}
void process()
{
    for (;;)
    {
        int poll_count = poll(pfds, 1, -1);

        if (poll_count == -1)
        {
            perror("poll");
            break;
        }

        if (!(pfds->revents & POLLIN))
        {
            continue;
        }

        struct sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        client_socket_fd = accept(socket_fd, (struct sockaddr *)&client, &clientSize);
        if (client_socket_fd < 0)
        {
            fprintf(stderr, "Error: Problem with client connecting ! %s\n", strerror(errno));
        }

        int processed = 0;

        memset(buffer, 0, buffer_size);

        int rv = recv(client_socket_fd, buffer, buffer_size, 0);

        if (rv < 0)
        {
            fprintf(stderr, "Error: Problem with recieving ! %s\n", strerror(errno));
            continue;
        }

        for (int i = 0; i < buffer_size && buffer[i]; i++)
        {
            if (buffer[i] == '{')
            {
                process_test_cases(buffer + i);
                processed = 1;
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

    free(pfds);

    pfds = NULL;

    free(buffer);

    buffer = NULL;
}

#endif // NIX_PROCESSOR_HPP