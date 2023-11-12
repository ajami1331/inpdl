/// Copyright (c) 2023 Araf Al Jami
#ifndef TESTCASES_H
#define TESTCASES_H

#include "options.h"
#include <stdio.h>
#include "cJson/cJson.h"
#include <ftw.h>
#include <unistd.h>

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}

int rmrf(const char *path)
{
    return ftw(path, unlink_cb, 64);
}

void process_test_cases(const char *body)
{
    cJSON *json = cJSON_Parse(body);
    cJSON *name = cJSON_GetObjectItem(json, "name");
    fprintf(stdout, "Name: %s\n", name->valuestring);
    cJSON *tests = cJSON_GetObjectItem(json, "tests");
    int case_num = 0;
    rmrf(test_case_dir);
    mkdir(test_case_dir, 0777);
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf) - 1, "%scnt", test_case_dir);
    FILE *cnt = fopen(buf, "w");
    fprintf(cnt, "%d", cJSON_GetArraySize(tests));
    fclose(cnt);
    cJSON *test = tests->child;
    while (test != NULL)
    {
        ++case_num;
        cJSON *input = cJSON_GetObjectItem(test, "input");
        cJSON *output = cJSON_GetObjectItem(test, "output");
        fprintf(stdout, "Input:\n%s\n", input->valuestring);
        fprintf(stdout, "Output:\n%s\n", output->valuestring);
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf) - 1, "%s%d.in", test_case_dir, case_num);
        FILE *in = fopen(buf, "w");
        fprintf(in, "%s", input->valuestring);
        fclose(in);
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf) - 1, "%s%d.val", test_case_dir, case_num);
        FILE *val = fopen(buf, "w");
        fprintf(val, "%s", output->valuestring);
        fclose(val);
        test = test->next;
    }
}

#endif // TESTCASES_H