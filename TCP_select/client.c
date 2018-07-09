#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int wait_server(int s);

int main(int argc, char *argv[])
{
    short port;
    int s, result;
    struct sockaddr_in saddr;
    struct in_addr addr;

    if(argc!=3){
        fprintf(stderr, "Syntax error: %s <address> <port>\n", argv[0]);
        exit(-1);
    }

    port = atoi(argv[2]);

    if((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){ //INVALID_SOCKET
        fprintf(stderr, "Socket failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons((uint16_t)port);
    if(inet_aton(argv[1], &saddr.sin_addr)==0){ //from "dotted decimal" to network byte order
        fprintf(stderr, "Conversion to network byte order failed. Error: string not valid\n");
        exit(-3);
    }

    fprintf(stdout, "Connecting to %s:%d\n", inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));
    if(connect(s, (struct sockaddr *)&saddr, sizeof(saddr))==-1){
        fprintf(stderr, "Connection failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    else
        fprintf(stdout, "Connected...\n");

    //request something to server

    fprintf(stdout, "Waiting for response...\n");
    if(wait_server(s)==0){ //no response from server after several times
        free2d(files, nfile);
        close_conn(s);
        exit(-4);
    }

    fprintf(stdout, "Closing connection...\n");
    if(close(s)!=0){
        fprintf(stderr, "Close failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    return 0;
}

int wait_server(int s)
{
    int i=0, activity, time=10, n=3;
    struct timeval tval;
    fd_set cset;

    for(i=0; i<n; i++){
        tval.tv_sec=time;
        tval.tv_usec=0;

        FD_ZERO(&cset);
        FD_SET(s, &cset);

        if((activity=select(FD_SETSIZE, &cset, NULL, NULL, &tval))==-1){
            fprintf(stderr, "select() failed\n");
            close_conn(s);
            exit(EXIT_FAILURE);
        }

        if(activity>0){ //something unblock the select
            if(FD_ISSET(s, &cset)){ //my socket unblock the select
                FD_CLR(s, &cset);
                return 1;
            }
        }
    }

    fprintf(stdout, "No response from server after %d s\n", time*n);

    FD_CLR(s, &cset);

    return 0;
}