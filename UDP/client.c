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
    ssize_t nsend, nread;
    short port;
    int s, result;
    char *buffer=malloc(N*sizeof(char));
    struct sockaddr_in saddr;
    struct in_addr addr;
    socklen_t fromlen;

    if(argc!=4){
        fprintf(stderr, "Syntax error: %s <address> <port> <message>\n", argv[0]);
        exit(-1);
    }

    if(inet_aton(argv[1], &addr)==0){ //from "dotted decimal" to network byte order
        fprintf(stderr, "Conversion to network byte order failed. Error: string not valid\n");
        exit(-2);
    }
    port = atoi(argv[2]);
    strcpy(buffer, argv[3]);

    s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s<0){ //INVALID_SOCKET
        fprintf(stderr, "Socket failed\n");
        exit(EXIT_FAILURE);
    }

    memset((char *)&addr, 0, sizeof(addr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = addr.s_addr;

    fprintf(stdout, "Sendign datagram with message: %s...\n", buffer);
    nsend = sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&saddr, sizeof(saddr));
    if(nsend==-1){
        fprintf(stderr, "Sending of the datagram failed\n");
        exit(EXIT_FAILURE);
    }

    memset(buffer, '\0', N); //clear the buffer by filling null, it might have previously received data
    fromlen = sizeof(struct sockaddr_in);

    nread = recvfrom(s, buffer, N, 0, (struct sockaddr *)&saddr, &fromlen);
    if(nread==-1){
        fprintf(stderr, "recvfrom() failed\n");
        exit(EXIT_FAILURE);
    }
    else
        fprintf(stdout, "Response from client: %s", buffer);

    free(buffer);

    fprintf(stdout, "Closing connection...\n");
    if(close(s)!=0){
        fprintf(stderr, "Close failed\n");
        exit(-3);
    }

    return 0;

}