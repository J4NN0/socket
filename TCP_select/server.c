#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BKLOG 10

int main(int argc, char *argv[])
{
    short port;
    int s1, s2, result;
    struct sockaddr_in saddr, caddr;
    socklen_t addrlen = sizeof(caddr);

    if(argc!=2){
        fprintf(stderr, "Syntax error: %s <address> <port>\n", argv[0]);
        exit(-1);
    }

    port = atoi(argv[1]);

    if((s1=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){ //INVALID_SOCKET
        fprintf(stderr, "Socket failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons((uint16_t)port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(s1, (struct sockaddr *)&saddr, sizeof(saddr))==-1){
        fprintf(stderr, "Bind on port %d failed. Error: %d\n", port, errno);
        exit(EXIT_FAILURE);
    }
    else
        fprintf(stdout, "Listener on port %d\n", port);

    if(listen(s1, BKLOG)<0){
        fprintf(stderr, "Listen failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while(1){
        fprintf(stdout, "Waiting for connection...\n");
        s2 = accept(s1, (struct sockaddr *)&caddr, &addrlen);
        fprintf(stdout, "Connection accepted %s:%d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));

        //clien request

        if(close(s2)!=0){
            fprintf(stderr, "Close failed\n");
        }
    }
}