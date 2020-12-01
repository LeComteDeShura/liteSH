#include "func.h"
#include "server.h"

void server(char* ip){
    int s, ns;
    int pid;
    int nport;
    struct sockaddr_in serv_addr, clnt_addr;
    // struct hostent* hp;
    char buf[1000];


    nport = PORTNUM;
    nport = htons((u_short)nport);


    if ((s=socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("Ошибка вызова socket()");
        exit(1);
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");


    bzero(&serv_addr, sizeof(serv_addr));
    struct in_addr inp;
    char lol[] = "192.168.1.48\0";
    inet_aton(lol, &inp);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inp.s_addr;
    serv_addr.sin_port = nport;

    if (bind(s, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) {
        perror("Ошибка вызова bind()");
        exit(1);
    }

    // fprintf(stderr, "Сервер готов: %s\n pid: %d \n", inet_ntoa(serv_addr.sin_addr), getpid());


    if (listen(s, 5)==-1) {
        perror("Ошибка вызова listen()");
        exit(1);
    }
    while (1) {
        int fds[2];
        char buf2[1000];
        pipe(fds);
        int rv;
        int comm;

        socklen_t addrlen;
        bzero(&clnt_addr, sizeof(clnt_addr));
        addrlen = sizeof(clnt_addr);

        if ((ns=accept(s, (struct sockaddr*)&clnt_addr, &addrlen))==-1) {
            perror("Ошибка вызова accept()");
            exit(1);
        }
        // fprintf(stderr, "Клиент = %s\n", inet_ntoa(clnt_addr.sin_addr));
        // std::cout << "pid: " << getpid() << '\n';

        if ((pid=fork())==-1) {
            perror("Ошибка вызова fork()");
            exit(1);
        }
        if (pid==0) {
            int nbytes;

            close(s);
            close(fds[0]);
            close(1);
            dup(fds[1]);
            dup2(fds[1], STDERR_FILENO);
            memset(buf, 0, 1000);
            while ((nbytes = recv(ns, buf, 1000, 0)) !=0) {
                close(ns);
                // fprintf(stderr, "child: %s %d\n", buf, nbytes);
                buf[nbytes] = '\0';
                int argc = 0;
                char** args = (char**)malloc(100);
                // fprintf(stderr, "child: %s\n", buf);
                char *cmdbuf = strdup(buf);
                comm = parse_command(cmdbuf, &argc, args);
                // fprintf(stderr, "child: %s %s %d\n", args[0], args[1], argc);
                args[argc] = 0;

                if (comm == CD) {
                    write(fds[1], buf, nbytes);
                    exit(0);
                }
                if (comm == WHEREAMI) {
                    write(fds[1], buf, nbytes);
                    exit(0);
                }
                if (comm == PROGRAM) {
                    execvp(args[0], args);
                }
                // fprintf(stderr, "child: %s\n", buf);
                exit(0);

            }

            exit(0);
        }

         close(fds[1]);
         wait(&rv);

         int count = read(fds[0], buf2, 1000);
         buf2[count] = 0;
         // fprintf(stderr, "buf: %s \ncount: %d\n", buf2, count);
         if (count == 0) {
             send(ns, "", 0, 0);
             close(ns);
             continue;
         }

         int argc;
         char** args = (char**)malloc(100);
         char *cmdbuf = strdup(buf2);
         comm = parse_command(cmdbuf, &argc, args);
         // std::cout << comm << '\n';
         // std::cout << buf2 << '\n';
         if (comm == CD) {
             chdir(args[1]);
             char *dir = (char*)malloc(MAXPATH);
             getcwd(dir, MAXPATH);
             int len = strlen(dir);
             // fprintf(stderr, "rd: %s %d\n", dir, len);
             send(ns, dir, len, 0);
             free(dir);
             close(ns);
             continue;
         }

         if (comm == WHEREAMI) {
             char *dir = (char*)malloc(MAXPATH);
             getcwd(dir, MAXPATH);
             int len = strlen(dir);
             // fprintf(stderr, "rd: %s %d\n", dir, len);
             send(ns, dir, len, 0);
             free(dir);
             close(ns);
             continue;
         }

         else{
             send(ns, buf2, count, 0);
             close(ns);
             continue;
         }
         memset(buf2, 0, 1000);
         close(ns);
    }
}
