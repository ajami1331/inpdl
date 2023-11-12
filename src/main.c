/// Copyright (c) 2023 Araf Al Jami
#include <stdio.h>
#include "options.h"
#include "nix_processor.h"

int watch_mode = 0;
char test_case_dir[1024] = "./test_cases/";

int main(int argc, char **argv)
{
    fprintf(stdout, "Copyright (c) 2023 Araf Al Jami\n");
    fprintf(stdout, "Starting inpdl listener...\n");

    process_opts(argc, argv);

    if (watch_mode)
    {
        fprintf(stdout, "Watch mode enabled\n");
    }

    nix_processor_init();
   
    process();

    nix_processor_shutdown();
    
    return 0;
}