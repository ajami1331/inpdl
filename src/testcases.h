/// Copyright (c) 2023 Araf Al Jami
#ifndef TESTCASES_H
#define TESTCASES_H

#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include "options.h"

namespace fs = std::filesystem;

void process_test_cases(const char *body)
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

#endif // TESTCASES_H