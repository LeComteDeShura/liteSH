#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
// #include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// #include <iostream>
#include <signal.h>
#include <sys/param.h>
#include <syslog.h>
#include <sys/resource.h>
#include <pwd.h>

/* Номер порта сервера, известный клиентам */
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

int parse_command(char* cmdbuf, int *argc, char** arguments){
    *argc = 0;
    // char *token;
    // while((token = strtok_r(cmdbuf, " ", &cmdbuf))) {
    //     arguments[*argc] = token;
    //     //token = strtok_r(NULL, " ");
    //     (*argc)++;
    // }

    char* token = strtok(cmdbuf, " ");

    while (token != NULL) {
        arguments[*argc] = token;
        token = strtok(NULL, " ");
        (*argc)++;
    }

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

int client(char *buf, char* result, char* ip, int size){
    int s;
    int pid;
    int i, j;
    struct sockaddr_in serv_addr;
    struct hostent *hp;
    // char buf[80];
    // char buf2[1000];

    /* В качестве аргумента клиенту передается доменное имя
       хоста, на котором запущен сервер. Произведем трансляцию
       доменного имени в адрес */
    if ((hp = gethostbyname(ip)) == 0) {
     perror("Ошибка вызова gethostbyname()");
     exit(3);
    }
    bzero(&serv_addr, sizeof(serv_addr));
    bcopy(hp->h_addr, &serv_addr.sin_addr, hp->h_length);
    serv_addr.sin_family = hp->h_addrtype;
    serv_addr.sin_port = htons(PORTNUM);

    /* Создадим сокет */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
     perror("Ошибка вызова socket!)");
     exit(1);
    }
    fprintf(stderr, "Адрес клиента: %s\n",
     inet_ntoa(serv_addr.sin_addr));

    /* Создадим виртуальный канал */
    if (connect (s, (struct sockaddr*)&serv_addr,
     sizeof(serv_addr)) == -1) {
     perror("Ошибка вызова connect()");
     exit(1);
    }
    /* Отправим серверу сообщение и получим его обратно */
    int nb;
    // std::cout << buf << '\n';
    // std::cout << "начал принимать" << '\n';
    send(s, buf, size, 0);
    buf[0] = '\0';
    if ((nb = recv(s, result, 1000, 0)) < 0) {
     perror("Ошибка вызова recv()");
     exit(1);
    }
    // printf("%s %d\n", result, nb);
    // std::cout << "закончил" << '\n';
    // std::cout << "/* message */" << '\n';

    // int cmdsize = read(0, buf3, CMDSIZE);
    // buf2[nb-1] ='\0';

    /* Выведем полученное сообщение на экран */
    // printf("Получено от сервера: %s\n", result);
    close(s);
    // printf("Клиент завершил работу \n\n");
    return nb;
}

int main (){
    int command;
    int stat_loc;
    char cmdpath[MAXPATH];
    int pid;
    int stat_lock;

    // char name[100];
    struct passwd *pw;
    uid_t uid;

    uid = geteuid();
    pw = getpwuid(uid);

    // char name = pw->pw_name();
    int len_name = strlen(pw->pw_name);

    while (1) {

        char dir[MAXPATH];
        getcwd(dir, MAXPATH);
        int len_dir = strlen(dir);

        // write(1, "\E[34m", 6);
        // write(1, dir, l);
        // write(1, "\E[39m$ ", 8);



        write(1, "\E[1;33m", 8);
        write(1, pw->pw_name, len_name);
        write(1, "\E[39m:", 7);
        // write(1, ":", 1);/asd
        write(1, "\E[34m", 6);
        write(1, dir, len_dir);
        write(1, "\E[0;39m$ ", 10);

        char cmd[CMDSIZE];

        int cmdsize = read(0, cmd, CMDSIZE);
        cmd[cmdsize-1] ='\0';

        char** args = (char**)malloc(100);
        int argc;
        command = parse_command(cmd, &argc, args);

        switch(command){
            case (CD):
                chdir(args[1]);
                break;

            case(SERVER):{

            }
            // {
                // pid = fork();
                //
                // if (pid < 0)
                //     write(2, "shell: cannot fork\n", 19);
                //
                // else if (pid == 0) {
                //     // args[argc-1] = 0;
                //     // struct rlimit flim;
                //     // if (getppid() != 1)
                //     //     signal(SIGTTOU, SIG_IGN);
                //     //     signal(SIGTTIN, SIG_IGN);
                //     //     signal(SIGTSTP, SIG_IGN);
                //
                //     setsid();
                //
                //     // close(STDIN_FILENO);
                //     // close(STDOUT_FILENO);
                //     // close(STDERR_FILENO);
                //
                //     server();
                // } else
                //     break;
                // }

            case(CLIENT):{
                system("./client");
                // char ip[16];
                // int len_ip = read(0, ip, 16);
                // ip[len_ip-1] ='\0';
                // std::cout << ip << '\n';
                // args[1] = "37.193.54.5\0";
                int size;
                int sizedir;
                char *dirs = (char*)malloc(1000);
                char whereami[] = "whereami\0";
                sizedir = client(whereami, dirs, args[1], strlen(whereami));
                // dir[size-1] ='\0';

                while (1) {
                    write(1, "\E[1;33m", 8);
                    write(1, args[1], strlen(args[1]));
                    write(1, "\E[39m:", 7);
                    // write(1, ":", 1);/asd
                    write(1, "\E[34m", 5);
                    write(1, dirs, sizedir);
                    write(1, "\E[0;39m$ ", 10);

                    char *buf3 = (char*)malloc(1000);
                    char *result = (char*)malloc(1000);
                    int cmdsize = read(0, buf3, CMDSIZE);
                    buf3[cmdsize-1] ='\0';

                    int num;
                    char** argscomm = (char**)malloc(100);
                    char *cmdbuf = strdup(buf3);
                    int command = parse_command(cmdbuf, &num, argscomm);
                    // if (buf3[0] == 'c' && buf3[1] == 'd') {
                    //     delete [] dir;
                    //     dir = new char[1000];
                    //     sizedir = client(buf3, dir);
                    // }
                    // if (buf3) {
                    //     /* code */
                    // }
                    // std::cout << command << '\n';
                    // std::cout << buf3 << '\n';
                    // std::cout << cmdsize << '\n';
                    if (command == EXIT) {
                        break;
                    }

                    if (command == CD) {
                        free(dirs);
                        dirs = (char*)malloc(1000);
                        sizedir = client(buf3, dirs, args[1], cmdsize);
                        // dir[sizedir] ='\0';
                        // printf("%s %d\n", dir, sizedir);
                    }

                    else{
                        size = client(buf3, result, args[1], cmdsize);
                        if (size == 0) {
                            continue;
                        }
                        result[size-1] ='\0';
                        printf("%s\n", result);
                        // free(bu/sult);
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
                    // getrlimit(RLIMIT_NOFILE, &flim);
                    // for (int fd = 0; fd < flim.rlim_max; fd++)
                    //     close(fd);
                    close(STDIN_FILENO);
                    close(STDOUT_FILENO);
                    close(STDERR_FILENO);
                    execvp(args[0], args);
                    // openlog("Скелет демона" , LOG_PID | LOG_CONS, LOG_DAEMON);
                    //
                    // syslog(LOG_INFO, "Демон начал плодотворную работу...");
                    // closelog();
                    // std::cout << "/* message */" << '\n';
                    while (1) {

                    }
                    // write(2, "shell: cannot execute\n", 22);
                } else
                    break;

                }
            case(EXIT):
                exit(0);
            }

            free(args);
        }
}
