#ifndef FUNC_H
#define FUNC_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
// #include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/param.h>
#include <syslog.h>
#include <sys/resource.h>
#include <pwd.h>


#define PORTNUM 1500
#define CD 1
#define EXIT 4
#define PROGRAM 5
#define DAEMON 6
#define SERVER 7
#define CLIENT 8
#define WHEREAMI 9

#define CMDSIZE 80
#define MAXPATH 1000

int parse_command(char* cmdbuf, int *argc, char** arguments);

#endif
