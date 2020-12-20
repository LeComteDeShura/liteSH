#include <sys/prctl.h>
#include <dlfcn.h>

#include "func.h"
#include "server.h"
#include "client.h"


int main (int n, char const *argv[]){
    if (n > 1) {
        if (!strcmp(argv[1], "--help")) {
            void *handle;
            void (*help)(void);
            char *error;
            handle = dlopen ("lib/help.so", RTLD_LAZY);
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
                int fds[2];
                // char buf2[1000];
                pipe(fds);
                pid = fork();

                if (pid < 0)
                    write(2, "shell: cannot fork\n", 19);

                else if (pid == 0) {

                    // if (getppid() != 1)
                    signal(SIGTTOU, SIG_IGN);
                    signal(SIGTTIN, SIG_IGN);
                    signal(SIGTSTP, SIG_IGN);

                    setsid();
                    // close(fds[0]);
                    // dup(fds[1]);
                    // char pidServer[] = itoa(getpid());
                    // write(fds[1], pidServer, 8);
                    prctl(PR_SET_NAME, "server3000");

                    server("123");


                }
                // close(fds[1]);
                // char pidServer[8];
                // read(fds[0], pidServer, 8);
                // printf("%s\n", pidServer);
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

                    char *ccmd = (char*)malloc(1000);
                    char *result = (char*)malloc(1000);
                    int ccmdsize = read(0, ccmd, CMDSIZE);
                    ccmd[ccmdsize-1] ='\0';

                    int num;
                    char** cargs = (char**)malloc(100);
                    char *cmdbuf = strdup(ccmd);
                    int command = parse_command(cmdbuf, &num, cargs);

                    if (command == EXIT) {
                        break;
                    }

                    if (command == CD) {
                        free(dirs);
                        dirs = (char*)malloc(1000);
                        sizedir = client(ccmd, dirs, args[1], ccmdsize);
                    }

                    else{
                        size = client(ccmd, result, args[1], ccmdsize);
                        if (size == 0) {
                            continue;
                        }
                        result[size-1] ='\0';
                        printf("%s\n", result);
                    }
                    free(ccmd);
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
                    argv[n-1] = 0;

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
                free(args);
                exit(0);
            }

            free(args);
        }
}
