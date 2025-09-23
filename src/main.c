/*
 * main.c - Example application for os-c-template
 *
 * Demonstrates basic usage of the `mylib` API:
 *  - initialization and shutdown
 *  - simple arithmetic helpers
 *  - option set/get
 *
 * Build:
 *   mkdir build && cd build
 *   cmake .. && cmake --build .
 *
 * Run:
 *   ./os_app --help
 *
 * This file is intended to be small, readable, and portable (C11).
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "mylib.h"

static void print_usage(const char* prog)
{
    printf(
        "Usage: %s <command> [args]\n"
        "Commands:\n"
        "  --help                   Show this help\n"
        "  --version                Print library and app version\n"
        "  add <a> <b>              Print a + b\n"
        "  sub <a> <b>              Print a - b (checked for overflow)\n"
        "  echo <string>            Duplicate and print a string via library\n"
        "  set <key> <value>        Set an option in library\n"
        "  get <key>                Get an option from library\n"
        "\n",
        prog);
}

/* Parse a 32-bit integer from string. Returns 0 on success. */
static int parse_int32(const char* s, int32_t* out)
{
    long  val = 0;
    char* end = NULL;

    if (!s || !out)
    {
        return -1;
    }

    errno = 0;
    val   = strtol(s, &end, 10);
    if (end == s || *end != '\0' || errno == ERANGE)
    {
        return -1;
    }
    if (val < INT32_MIN || val > INT32_MAX)
    {
        return -1;
    }
    *out = (int32_t)val;
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* Initialize the library */
    if (mylib_init() != MYLIB_OK)
    {
        fprintf(stderr, "Failed to initialize library\n");
        return EXIT_FAILURE;
    }

    const char* cmd = argv[1];

    if (strcmp(cmd, "--help") == 0 || strcmp(cmd, "-h") == 0)
    {
        print_usage(argv[0]);
        mylib_shutdown();
        return EXIT_SUCCESS;
    }
    else if (strcmp(cmd, "--version") == 0)
    {
        printf("app: os_app\n");
        printf("mylib: %s\n", mylib_version_string());
        mylib_shutdown();
        return EXIT_SUCCESS;
    }
    else if (strcmp(cmd, "add") == 0)
    {
        if (argc != 4)
        {
            fprintf(stderr, "add requires 2 arguments\n");
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        int32_t a = 0, b = 0;
        if (parse_int32(argv[2], &a) != 0 || parse_int32(argv[3], &b) != 0)
        {
            fprintf(stderr, "Invalid integer arguments for add\n");
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        int res = mylib_add((int)a, (int)b);
        printf("%d\n", res);
    }
    else if (strcmp(cmd, "sub") == 0)
    {
        if (argc != 4)
        {
            fprintf(stderr, "sub requires 2 arguments\n");
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        int32_t a = 0, b = 0, out = 0;
        if (parse_int32(argv[2], &a) != 0 || parse_int32(argv[3], &b) != 0)
        {
            fprintf(stderr, "Invalid integer arguments for sub\n");
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        mylib_error_t err = mylib_subtract_checked(a, b, &out);
        if (err != MYLIB_OK)
        {
            fprintf(stderr, "sub failed: %s\n", mylib_error_string(err));
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        printf("%" PRId32 "\n", out);
    }
    else if (strcmp(cmd, "echo") == 0)
    {
        if (argc != 3)
        {
            fprintf(stderr, "echo requires 1 argument\n");
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        char* dup = mylib_dup_string(argv[2]);
        if (!dup)
        {
            fprintf(stderr, "mylib_dup_string failed\n");
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        puts(dup);
        mylib_free_string(dup);
    }
    else if (strcmp(cmd, "set") == 0)
    {
        if (argc != 4)
        {
            fprintf(stderr, "set requires <key> <value>\n");
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        mylib_error_t err = mylib_set_option(argv[2], argv[3]);
        if (err != MYLIB_OK)
        {
            fprintf(stderr, "Failed to set option: %s\n", mylib_error_string(err));
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        printf("OK\n");
    }
    else if (strcmp(cmd, "get") == 0)
    {
        if (argc != 3)
        {
            fprintf(stderr, "get requires <key>\n");
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        char*         val = NULL;
        mylib_error_t err = mylib_get_option(argv[2], &val);
        if (err != MYLIB_OK)
        {
            fprintf(stderr, "Failed to get option: %s\n", mylib_error_string(err));
            mylib_shutdown();
            return EXIT_FAILURE;
        }
        if (val)
        {
            puts(val);
            mylib_free_string(val);
        }
        else
        {
            printf("(null)\n");
        }
    }
    else
    {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        print_usage(argv[0]);
        mylib_shutdown();
        return EXIT_FAILURE;
    }

    /* Shutdown library and exit */
    mylib_shutdown();
    return EXIT_SUCCESS;
}
