/*
 * Copyright (c) 2022 ACOINTO Team.
 * All rights reserved.
 *
 * Detailed license information can be found in the LICENSE file.
 *
 * File: xtrace.c trace command line
 *
 * Author: Jiang.Taijin <jiangtaijin@acoinfo.com>
 *
 */

#include <stdio.h>
#include <string.h>
#include "../xtrace/xtrace-api.h"

/* Trace command line */
int main (int argc, char **argv)
{
    int arg_ops = 2;
    int trace_mode = TRACE_TYPE_MEMORY;
    int buffer_size = 0;
    char *dir = NULL;
    char abs_dir[PATH_MAX] = {0};
    char cwd[PATH_MAX] = {0};

    if (argc < 2) {
        printf("xtrace cmd format:\n");
        printf("xtrace operator [options], e.g:\n");
        printf("    xtrace create -m memory -s 1m|32k|409600\n");
        printf("    xtrace create -m file -s 1m|32k|409600 -d ./\n");
        printf("    xtrace start\n");
        printf("    xtrace stop\n");
        printf("    xtrace enable all|sched|int|memory|disk\n");
        printf("    xtrace disable all|sched|int|memory|disk\n");
        printf("    xtrace save ./\n");
        printf("    xtrace destroy\n");
        return  (TRACE_ERROR);
    }

    if (strcmp(argv[1], "create") == 0) {
        while ((arg_ops + 1) < argc) {
            if (strcmp(argv[arg_ops], "-m") == 0) {
                if (strcmp(argv[arg_ops + 1], "memory") == 0) {
                    trace_mode = TRACE_TYPE_MEMORY;
                } else if (strcmp(argv[arg_ops + 1], "file") == 0) {
                    trace_mode = TRACE_TYPE_FILE;
                } else {
                    printf("trace mode error!\n");
                    return  (TRACE_ERROR);
                }
            } else if (strcmp(argv[arg_ops], "-s") == 0) {
                sscanf(argv[arg_ops + 1], "%u", &buffer_size);
                if (lib_strchr(argv[arg_ops + 1], 'M') || lib_strchr(argv[arg_ops + 1], 'm')) {
                    buffer_size = buffer_size * (unsigned int)(1024 * 1024);
                } else if (lib_strchr(argv[arg_ops + 1], 'K') || lib_strchr(argv[arg_ops + 1], 'k')) {
                    buffer_size = buffer_size * (unsigned int)1024;
                }
            } else if (strcmp(argv[arg_ops], "-d") == 0) {
                dir = argv[arg_ops + 1];
                if (dir[0] == '.') {
                    snprintf(abs_dir, PATH_MAX, "%s/%s", getcwd(cwd, PATH_MAX), dir + 1);
                } else {
                    strlcpy(abs_dir, dir, sizeof(abs_dir));
                }
            } else {
                printf("trace arument error!\n");
                return  (TRACE_ERROR);
            }

            arg_ops += 2;
        }

        if (buffer_size == 0) {
            printf("trace buffer size error!\n");
        }

        if (trace_create(trace_mode, buffer_size, (dir ? abs_dir : NULL)) != TRACE_OK) {
            printf("trace create failed!\n");
        }

    } else if (strcmp(argv[1], "destroy") == 0) {
        trace_destroy();

    } else if (strcmp(argv[1], "start") == 0) {
        if (trace_start() != TRACE_OK) {
            printf("trace start failed!\n");
        }

    } else if (strcmp(argv[1], "stop") == 0) {
        trace_stop();

    } else if (strcmp(argv[1], "save") == 0) {
        if (argc < 3) {
            printf("param error!\n");
        }

        if (trace_save(argv[2]) != TRACE_OK) {
            printf("trace save failed!\n");
        }
    } else if (strcmp(argv[1], "enable") == 0) {
        if (argc < 3) {
            printf("param error!\n");
        }

        trace_enable(argv[2], 1);
    } else if (strcmp(argv[1], "disable") == 0) {
        if (argc < 3) {
            printf("param error!\n");
        }

        trace_enable(argv[2], 0);
    } else {
        printf("xtrace cmd format:\n");
        printf("xtrace operator [options], e.g:\n");
        printf("    xtrace create -m memory -s 1m|32k|409600\n");
        printf("    xtrace create -m file -s 1m|32k|409600 -d ./\n");
        printf("    xtrace start\n");
        printf("    xtrace stop\n");
        printf("    xtrace enable all|sched|int|memory|disk\n");
        printf("    xtrace save ./\n");
    }

    return  (TRACE_OK);
}
