#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define N 31

int main(int argc, char *argv[])
{
    ssize_t nread, nsed;
    short port;
    int s, result;
    char *buffer=malloc(N*sizeof(char));
    struct sockaddr_in saddr;
    socklen_t addrlen;
    socklen_t fromlen;

    if(argc!=2){
        fprintf(stderr, "Syntax error: %s <port>\n", argv[0]);
        exit(-1);
    }

    port = atoi(argv[1]);

    s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s<0){
        fprintf(stderr, "Socket failed\n");
        exit(EXIT_FAILURE);
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if(result==-1){
        fprintf(stderr, "Bind failed.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Waiting for data...\n");
    fromlen = sizeof(struct sockaddr_in);
    nread = recvfrom(s, buffer, N, 0, (struct sockaddr *)&saddr, &fromlen);
    if(nread==-1){
        fprintf(stderr, "recvfrom() failed\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Received packet from %s:%d\n", inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));
    fprintf(stdout, "Data: %s\n", buffer);

    strcpy(buffer, "fuck");
    nsed = sendto(s, buffer, nread, 0, (struct sockaddr *)&saddr, sizeof(saddr));
    if(nsed==-1){
        fprintf(stderr, "sendto() failed\n");
        exit(EXIT_FAILURE);
    }

    free(buffer);

    if(close(s)!=0){
        fprintf(stderr, "Close failed\n");
        exit(-3);
    }

    return 0;
}