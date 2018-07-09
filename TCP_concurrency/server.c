#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BKLOG 15 //backlog

static int nchild=0;

void sigHand(int signo);
void wait_child(int nchild);

int main(int argc, char *argv[])
{
    int s1, s2;
    short port;
    struct sockaddr_in saddr, caddr;
    socklen_t addrlen = sizeof(caddr);
    pid_t pid;

    signal(SIGINT, sigHand); //signal handler
    signal(SIGCHLD, sigHand);

    if(argc!=2){
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
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

    if(listen(s1, BKLOG)<0){
        fprintf(stderr, "Listen failed. Error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while(1){
        fprintf(stdout, "Waiting for a new connection...\n");
        s2 = accept(s1, (struct sockaddr *)&caddr, &addrlen);
        fprintf(stdout, "Connection accepted %s:%d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));

        if((pid=fork())<0){
            fprintf(stderr, "Fork failed\n");
            exit(EXIT_FAILURE);
        }
        else if(pid==0){ //child
            if(close(s1)!=0){ //closing socket's father
                fprintf(stderr, "Close failed. Error: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            fprintf(stdout, "Process generated. PID: %d\n", getpid());
            
            //client request

            fprintf(stdout, "Closing connection...\n");
            if(close(s2)!=0){
                fprintf(stderr, "Close failed. Error: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        else{ //father
            nchild++;
            if(close(s2)!=0){ //closing new socket
                fprintf(stderr, "Close failed. Error: %d\n", errno);
            }
        }
    }
}

void wait_child(int nchild)
{
    int i=0, status=0;
    pid_t childpid;

    for(i=0; i<nchild; i++){
        childpid = wait(&status);
        if(status!=EXIT_SUCCESS)
            fprintf(stdout, " [!] An error occurred in child %d. Termination status: %d\n", childpid, status);
    }

    fprintf(stdout, "All process are terminated\n");
}

void sigHand(int signo)
{
    if(signo==SIGINT){ //CTRL-C
        fprintf(stdout, "Received signal %d: Interactive attention request sent to the program\n", signo);
        fprintf(stdout, "[>] Possible memory leaks\n");

        fprintf(stdout, "Waiting for all process child before quitting...\n");
        fprintf(stdout, "Press CTRL-C again to quit definitively...\n");
        wait_child(nchild);
        exit(1);
    }

    if(signo==SIGCHLD){ //Child call exit()
        int status, pid;

        fprintf(stdout, "Waiting for child...\n");
		while((pid = waitpid(-1, &status, WNOHANG))>0 && nchild>0){
        	nchild--;
        	fprintf(stdout, "%d return with status %d\n", pid, status);
		}
    }
}
