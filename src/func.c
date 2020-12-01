#include "func.h"

int parse_command(char* cmdbuf, int *argc, char** arguments){
    *argc = 0;

    // char* token = strtok(cmdbuf, " ");
    //
    // while (token != NULL) {
    //     arguments[*argc] = token;
    //     token = strtok(NULL, " ");
    //     (*argc)++;
    // }
    // fprintf(stderr, "func1: %s \n", cmdbuf);
    // char str[] = "Geeks for Geeks";
    char* token;
    char* rest = cmdbuf;

    while ((token = strtok_r(rest, " ", &rest))){
        // printf("%s\n", token);
        arguments[*argc] = token;
        // token = strtok(NULL, " ");
        (*argc)++;
    }
    // return (0);
    arguments[*argc] = 0;
    // fprintf(stderr, "func2: %s %s %s %d \n", cmdbuf, arguments[0], arguments[1], *argc);

    if (!strcmp("cd", arguments[0])) {
        return CD;
    }

    if (!strcmp("whereami", arguments[0])) {
        return WHEREAMI;
    }

    if (!strcmp("server", arguments[0])) {
        return SERVER;
    }

    if (!strcmp("client", arguments[0])) {
        return CLIENT;
    }
    //
    if (!strcmp("exit", arguments[0])) {
        return EXIT;
    }

    if (!strcmp("&", arguments[*argc-1])) {
        return DAEMON;
    }

    return PROGRAM;
}
