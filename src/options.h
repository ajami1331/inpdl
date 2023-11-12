/// Copyright (c) 2023 Araf Al Jami
#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include "ketopt.h"

extern int watch_mode;
extern char test_case_dir[1024];

void process_opts(int argc, char **argv)
{
    ketopt_t opt = KETOPT_INIT;
    int c, len;
    while ((c = ketopt(&opt, argc, argv, 1, "wd:", NULL)) > -1)
    {
        switch (c)
        {
        case 'w':
            watch_mode = 1;
            break;
        case 'd':
            strncpy(test_case_dir, opt.arg, sizeof(test_case_dir));
            len = strlen(test_case_dir);
            if (test_case_dir[len - 1] != '/' && test_case_dir[len - 1] != '\\')
            {
                test_case_dir[len] = '/';
                test_case_dir[len + 1] = '\0';
            }
            break;
        default:
            fprintf(stdout, "Usage: %s [-w] [-d test_case_dir]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

#endif // OPTIONS_H