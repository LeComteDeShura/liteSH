#include "func.h"
#include "client.h"

int client(char *buf, char* result, char* ip, int size){
    int s;
    int pid;
    int i, j;
    struct sockaddr_in serv_addr;
    struct hostent *hp;

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

    /* Создадим виртуальный канал */
    if (connect (s, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
     perror("Ошибка вызова connect()");
     exit(1);
    }

    int nb;
    send(s, buf, size, 0);
    buf[0] = '\0';
    if ((nb = recv(s, result, 1000, 0)) < 0) {
     perror("Ошибка вызова recv()");
     exit(1);
    }

    close(s);
    return nb;
}
