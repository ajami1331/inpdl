/// Copyright (c) 2023 Araf Al Jami
#include "options.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ketopt.h"

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
            snprintf(test_case_dir, sizeof(test_case_dir), "%s", opt.arg);
            len = strlen(test_case_dir);
            if (len + 1 >= (int)sizeof(test_case_dir))
            {
                fprintf(stderr, "Error: path too long\n");
                exit(EXIT_FAILURE);
            }
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
