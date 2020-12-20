#include "func.h"

int parse_command(char* cmdbuf, int *argc, char** arguments){
    *argc = 0;

    char* token;
    char* rest = cmdbuf;

    while ((token = strtok_r(rest, " ", &rest))){
        // printf("%s\n", token);
        arguments[*argc] = token;
        // token = strtok(NULL, " ");
        (*argc)++;
    }
    
    arguments[*argc] = 0;

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
