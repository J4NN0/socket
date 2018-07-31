#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BKLOG 10
#define BUFFSIZE (255+1)

int main(int argc, char *argv[])
{
    short port;
    int s1, s2, result;
    char buffer[BUFFSIZE];
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

        memset(buffer, '\0', BUFFSIZE);
        res = recv(s, buffer, BUFFSIZE, 0);
        if(res==0){
            fprintf(stdout, "Client close connection prematurely\n");    
        }
        else if(res>0){
            fprintf(stdout, "Client sent %s\n", buffer);
            memset(buffer, '\0', BUFFSIZE);
            strcpy(buffer, "Hello from server");
            if(send(s, buffer, strlen(buffer), 0)<0){ //sending header
                fprintf(stderr, "Send failed. Error: %d\n", errno);
                close(s);
                exit(EXIT_FAILURE);
            }
        }
        else{
            fprintf(stderr, "Recv failed. Error: %d\n", errno);
            close(s);
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "Closing connection...\n");
        if(close(s2)!=0){
            fprintf(stderr, "Close failed\n");
        }
    }
}