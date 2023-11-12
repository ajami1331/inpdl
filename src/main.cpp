/// Copyright (c) 2023 Araf Al Jami
#include <iostream>
#include "options.h"
#include "processor.hpp"
#ifdef __unix__
    #include "nix_processor.hpp"
    #define PlatformProcessor NixProcessor
#elif defined(_WIN32) || defined(WIN32)
    #include "win_processor.hpp"
    #define PlatformProcessor WinProcessor
#else
    #error "Unsupported platform"
#endif

bool watch_mode = false;
std::string test_case_dir = "./test_cases/";
Processor *processor = nullptr;

int main(int argc, char **argv)
{
    std::cout << "Copyright (c) 2023 Araf Al Jami" << std::endl;
    std::cout << "Starting inpdl listener..." << std::endl;

    process_opts(argc, argv);

    if (watch_mode)
    {
        std::cout << "Watch mode enabled" << std::endl;
    }

    processor = new PlatformProcessor();

    if (processor == nullptr)
    {
        std::cerr << "Error: couldn't start" << std::endl;
        exit(-1);
    }

    processor->Process();

    if (processor)
    {
        delete processor;
    }
    
    return 0;
}