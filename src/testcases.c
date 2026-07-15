/// Copyright (c) 2023 Araf Al Jami
#include "testcases.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "cJson/cJson.h"
#include <ftw.h>
#include <unistd.h>

static int unlink_cb(const char *fpath, const struct stat *sb, int typeflag)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return 0;
}

static void rmrf(const char *path)
{
    ftw(path, unlink_cb, 64);
}

void process_test_cases(const char *body)
{
    cJSON *json = cJSON_Parse(body);
    if (!json)
    {
        fprintf(stderr, "Error: failed to parse JSON body\n");
        return;
    }

    cJSON *name = cJSON_GetObjectItem(json, "name");
    if (!name || !name->valuestring)
    {
        fprintf(stderr, "Error: missing 'name' field in JSON\n");
        cJSON_Delete(json);
        return;
    }
    fprintf(stdout, "Name: %s\n", name->valuestring);

    cJSON *tests = cJSON_GetObjectItem(json, "tests");
    if (!tests || !cJSON_IsArray(tests))
    {
        fprintf(stderr, "Error: missing or invalid 'tests' field in JSON\n");
        cJSON_Delete(json);
        return;
    }

    int case_num = 0;
    rmrf(test_case_dir);
    if (mkdir(test_case_dir, 0777) != 0)
    {
        fprintf(stderr, "Error: failed to create directory '%s'\n", test_case_dir);
        cJSON_Delete(json);
        return;
    }

    char buf[2048];
    snprintf(buf, sizeof(buf), "%scnt", test_case_dir);
    FILE *cnt = fopen(buf, "w");
    if (!cnt)
    {
        fprintf(stderr, "Error: failed to open '%s'\n", buf);
        cJSON_Delete(json);
        return;
    }
    fprintf(cnt, "%d", cJSON_GetArraySize(tests));
    fclose(cnt);

    cJSON *test = tests->child;
    while (test != NULL)
    {
        ++case_num;
        cJSON *input = cJSON_GetObjectItem(test, "input");
        cJSON *output = cJSON_GetObjectItem(test, "output");

        if (!input || !input->valuestring || !output || !output->valuestring)
        {
            fprintf(stderr, "Warning: test case %d missing 'input' or 'output'\n", case_num);
            test = test->next;
            continue;
        }

        fprintf(stdout, "Input:\n");
        fwrite(input->valuestring, 1, strlen(input->valuestring), stdout);
        fprintf(stdout, "Output:\n");
        fwrite(output->valuestring, 1, strlen(output->valuestring), stdout);

        snprintf(buf, sizeof(buf), "%s%d.in", test_case_dir, case_num);
        FILE *in = fopen(buf, "w");
        if (in)
        {
            fwrite(input->valuestring, 1, strlen(input->valuestring), in);
            fclose(in);
        }
        else
        {
            fprintf(stderr, "Error: failed to open '%s'\n", buf);
        }

        snprintf(buf, sizeof(buf), "%s%d.val", test_case_dir, case_num);
        FILE *val = fopen(buf, "w");
        if (val)
        {
            fwrite(output->valuestring, 1, strlen(output->valuestring), val);
            fclose(val);
        }
        else
        {
            fprintf(stderr, "Error: failed to open '%s'\n", buf);
        }

        test = test->next;
    }

    cJSON_Delete(json);
}
