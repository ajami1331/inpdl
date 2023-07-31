/// Copyright (c) 2023 Araf Al Jami
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

const int buffer_size = 4096;

bool watch_mode = false;
char buffer[buffer_size];
std::string test_case_dir = "./test_cases/";

namespace fs = std::filesystem;

void process(const char *body)
{
    auto json = nlohmann::json::parse(body);
    std::cout << "Name: " << json["name"] << std::endl;
    fs::remove_all(test_case_dir);
    fs::create_directory(test_case_dir);
    std::ofstream out(test_case_dir + "cnt");
    out << json["tests"].size();
    out.close();
    int case_num = 0;
    for (const auto &it : json["tests"])
    {
        std::cout << "Input:" << it["input"] << std::endl;
        std::cout << "Output:" << it["output"] << std::endl;
        ++case_num;
        std::ofstream in(test_case_dir + std::to_string(case_num) + ".in");
        in << it["input"].get<std::string>();
        in.close();
        std::ofstream val(test_case_dir + std::to_string(case_num) + ".val");
        val << it["output"].get<std::string>();
        val.close();
    }
}

void shutdown()
{
    std::cout << "Shutting down inpdl listener..." << std::endl;
}

void process_opts(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "wd:")) != -1)
    {
        switch (opt)
        {
        case 'w':
            watch_mode = true;
            break;
        case 'd':
            test_case_dir = optarg;
            break;
        default:
            std::cout << "Usage: " << argv[0] << " [-w] [-d test_case_dir]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv)
{
    std::cout << "Starting inpdl listener..." << std::endl;

    process_opts(argc, argv);

    atexit(shutdown);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        std::cout << "Error: socket creation failed" << std::endl;
        return -1;
    }

    const int enable = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        std::cout << "Error: setsockopt(SO_REUSEADDR) failed" << std::endl;
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(4244);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(socket_fd, (struct sockaddr *)&hint, sizeof(hint)) < 0)
    {
        std::cerr << "Can't bind to IP/port";
        return -1;
    }

    if (listen(socket_fd, SOMAXCONN) < 0)
    {
        std::cerr << "Can't listen !";
        return -1;
    }

    for (;;)
    {
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int client_socket_fd = accept(socket_fd, (sockaddr *)&client, &clientSize);
        if (client_socket_fd < 0)
        {
            std::cerr << "Problem with client connecting !" << std::endl;
        }

        bool processed = false;

        recv(client_socket_fd, buffer, buffer_size, 0);

        for (int i = 0; i < buffer_size && buffer[i]; i++)
        {
            if (buffer[i] == '{')
            {
                process(buffer + i);
                processed = true;
                break;
            }
        }

        close(client_socket_fd);

        if (!watch_mode && processed)
        {
            break;
        }
    }

    close(socket_fd);

    return 0;
}