/// Copyright (c) 2023 Araf Al Jami
#include "nix_processor.h"
#include "testcases.h"
#include "options.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

static struct pollfd *pfds = NULL;
static int socket_fd = -1;
static int client_socket_fd = -1;
static const int buffer_size = 4096 * 4096;
static char *buffer;

void nix_processor_init(void)
{
    buffer = malloc(buffer_size);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        fprintf(stderr, "Error: socket creation failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    const int enable = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        fprintf(stderr, "Error: setsockopt(SO_REUSEADDR) failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(4244);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(socket_fd, (struct sockaddr *)&hint, sizeof(hint)) < 0)
    {
        fprintf(stderr, "Error: bind failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, SOMAXCONN) < 0)
    {
        fprintf(stderr, "Error: listen failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pfds = malloc(sizeof(struct pollfd));

    pfds->fd = socket_fd;
    pfds->events = POLLIN;
}

void nix_processor_shutdown(void)
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

void process(void)
{
    const char *http_ok_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 2\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n"
        "OK";

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
            continue;
        }

        int processed = 0;

        memset(buffer, 0, buffer_size);

        int rv = recv(client_socket_fd, buffer, buffer_size, 0);

        if (rv < 0)
        {
            fprintf(stderr, "Error: Problem with receiving ! %s\n", strerror(errno));
            close(client_socket_fd);
            client_socket_fd = -1;
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

        if (send(client_socket_fd, http_ok_response, strlen(http_ok_response), 0) < 0)
        {
            fprintf(stderr, "Error: Problem with sending HTTP response! %s\n", strerror(errno));
        }
        else
        {
            fprintf(stdout, "Sent 200 OK response.\n");
        }

        close(client_socket_fd);

        client_socket_fd = -1;

        if (!watch_mode && processed)
        {
            break;
        }
    }
}
