#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <rpc/xdr.h>

#define BUFFSIZE 256
#define N 80

int main(int argc, char *argv[])
{
    short port;
    int s, n1, n2, r1, r2, res;
    char buffer[BUFFSIZE], str[N];
    struct sockaddr_in saddr;
    XDR xdr_r, xdr_w; //pointer to XDR stream
    FILE *fp_xdr;

    if(argc!=3){
        fprintf(stderr, "Syntax error: %s <address> <port>\n", argv[0]);
        exit(-1);
    }

    port = atoi(argv[2]);

    if((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){ //INVALID_SOCKET
        fprintf(stderr, "Socket failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    if(inet_aton(argv[1], &saddr.sin_addr)==0){ //from "dotted decimal" to network byte order
        fprintf(stderr, "Conversion to network byte order failed. Error: string not valid\n");
        exit(-2);
    }

    if(connect(s, (struct sockaddr *)&saddr, sizeof(saddr))==-1){
        fprintf(stderr, "Connect failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    else
        fprintf(stdout, "Connected...\n");

    fp_xdr = fdopen(s, "r");
    if(fp_xdr == NULL){
        fprintf(stderr, "Error opening xdr file");
        exit(-2);
    }

    xdrmem_create(&xdr_w, buffer, BUFFSIZE, XDR_ENCODE); //create stream
    xdrstdio_create(&xdr_r, fp_xdr, XDR_DECODE);

    fprintf(stdout, "Insert two numebers: ");
    fgets(str, N, stdin);
    sscanf(str, "%d %d", &n1, &n2);

    r1 = xdr_int(&xdr_w, &n1);
    r2 = xdr_int(&xdr_w, &n2);

    if(r1==0 || r2==0){
        fprintf(stderr, "Error xdr write");
        exit(-3);
    }

    fprintf(stdout, "Sending %s to %s\n", str, argv[1]);
    if(send(s, buffer, xdr_getpos(&xdr_w), 0)<=0){
        fprintf(stderr, "Send failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    else{
        fprintf(stdout, "Send success\n");
    }

    if(!xdr_int(&xdr_r, &res))
        fprintf(stdout, "Invalid asnwer\n");
    else
        fprintf(stdout, "The result is: %d\n", res);

    xdr_destroy(&xdr_w);
    xdr_destroy(&xdr_r);

    if(close(s)!=0){
        fprintf(stderr, "Close failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    fclose(fp_xdr);

    return 0;

}