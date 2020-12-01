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
#include <sys/prctl.h>
#include <dlfcn.h>

#include "func.h"
#include "server.h"
#include "client.h"

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


int main (int n, char const *argv[]){
    if (n > 1) {
        if (!strcmp(argv[1], "--help")) {
            void *handle;
            void (*help)(void);
            char *error;
            handle = dlopen ("../lib/help.so", RTLD_LAZY);
            if (!handle) {
                fputs (dlerror(), stderr);
                exit(1);
            }
            help = dlsym(handle, "help");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "%s\n", error);
                exit(1);
            }
            help();
            dlclose(handle);
        }
        return 0;
    }


    signal(SIGINT, SIG_IGN);
    int command;
    int stat_loc;
    char cmdpath[MAXPATH];
    int pid;
    int stat_lock;

    struct passwd *pw;
    uid_t uid;

    uid = geteuid();
    pw = getpwuid(uid);

    int len_name = strlen(pw->pw_name);

    while (1) {
        char dir[MAXPATH];
        getcwd(dir, MAXPATH);
        int len_dir = strlen(dir);

        write(1, "\E[1;33m", 8);
        write(1, pw->pw_name, len_name);
        write(1, "\E[39m:", 7);

        write(1, "\E[34m", 6);
        write(1, dir, len_dir);
        write(1, "\E[0;39m$ ", 10);

        char cmd[CMDSIZE];

        int cmdsize = read(0, cmd, CMDSIZE);
        cmd[cmdsize-1] ='\0';

        char** args = (char**)malloc(100);
        int argc;
        // std::cout << cmdsize << '\n';
        if (cmd[0] == '\0' && cmdsize == 1) {
            continue;
        }
        command = parse_command(cmd, &argc, args);
        // std::cout << command << '\n';
        // std::cout << cmd << '\n';
        // std::cout << args[0] << '\n';
        switch(command){
            case (CD):
                chdir(args[1]);
                break;

            case(SERVER):
            {
            // std::cout << "/* message */" << '\n';
                pid = fork();

                if (pid < 0)
                    write(2, "shell: cannot fork\n", 19);

                else if (pid == 0) {

                    // struct rlimit flim;
                    // if (getppid() != 1)
                    signal(SIGTTOU, SIG_IGN);
                    signal(SIGTTIN, SIG_IGN);
                    signal(SIGTSTP, SIG_IGN);

                    setsid();
                    prctl(PR_SET_NAME, "myServer");
                    // std::cout << "\npid: " << getpid() << '\n';
                    // close(STDIN_FILENO);
                    // close(STDOUT_FILENO);
                    // close(STDERR_FILENO);
                    server("123");


                } else
                break;
            }
            break;

            case(CLIENT):{
                // system("./client");
                // args[1] = "192.168.1.48\0";
                // char ip = args[1];
                int size;
                int sizedir;
                char *dirs = (char*)malloc(1000);
                char whereami[] = "whereami\0";
                sizedir = client(whereami, dirs, args[1], strlen(whereami));


                while (1) {
                    write(1, "\E[1;33m", 8);
                    write(1, args[1], strlen(args[1]));
                    write(1, "\E[39m:", 7);

                    write(1, "\E[34m", 5);
                    write(1, dirs, sizedir);
                    write(1, "\E[0;39m$ ", 10);

                    char *buf3 = (char*)malloc(1000);
                    char *result = (char*)malloc(1000);
                    int ccmdsize = read(0, buf3, CMDSIZE);
                    buf3[ccmdsize-1] ='\0';

                    int num;
                    char** argscomm = (char**)malloc(100);
                    char *cmdbuf = strdup(buf3);
                    int command = parse_command(cmdbuf, &num, argscomm);

                    if (command == EXIT) {
                        break;
                    }

                    if (command == CD) {
                        free(dirs);
                        dirs = (char*)malloc(1000);
                        sizedir = client(buf3, dirs, args[1], ccmdsize);
                    }

                    else{
                        size = client(buf3, result, args[1], ccmdsize);
                        if (size == 0) {
                            continue;
                        }
                        result[size-1] ='\0';
                        printf("%s\n", result);
                    }
                    free(buf3);
                    free(result);
                }
                break;
            }

            case(PROGRAM):
                pid = fork();

                if (pid < 0)
                    write(2, "shell: cannot fork\n", 19);

                else if (pid == 0) {
                    execvp(args[0], args);
                    write(2, "shell: cannot execute\n", 22);
                } else
                    wait(&stat_lock);
                break;


            case(DAEMON):{
                pid = fork();

                if (pid < 0)
                    write(2, "shell: cannot fork\n", 19);

                else if (pid == 0) {
                    args[argc-1] = 0;
                    struct rlimit flim;
                    if (getppid() != 1)
                        signal(SIGTTOU, SIG_IGN);
                        signal(SIGTTIN, SIG_IGN);
                        signal(SIGTSTP, SIG_IGN);

                    setsid();

                    close(STDIN_FILENO);
                    close(STDOUT_FILENO);
                    close(STDERR_FILENO);
                    execvp(args[0], args);
                } else
                    break;

                }
            case(EXIT):
                exit(0);
            }

            free(args);
        }
}
