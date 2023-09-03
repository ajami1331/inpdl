/// Copyright (c) 2023 Araf Al Jami
#ifndef OPTIONS_H
#define OPTIONS_H

#include <iostream>
#include <string>
#include "ketopt.h"

extern bool watch_mode;
extern std::string test_case_dir;

void process_opts(int argc, char **argv)
{
    ketopt_t opt = KETOPT_INIT;
    int c;
    while ((c = ketopt(&opt, argc, argv, 1, "wd:", NULL)) > -1)
    {
        switch (c)
        {
        case 'w':
            watch_mode = true;
            break;
        case 'd':
            test_case_dir = opt.arg;
            if (test_case_dir.back() != '/' && test_case_dir.back() != '\\')
            {
                test_case_dir += '/';
            }
            break;
        default:
            std::cout << "Usage: " << argv[0] << " [-w] [-d test_case_dir]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

#endif // OPTIONS_H