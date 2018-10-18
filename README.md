# Socket
A series of examples of TCP, UDP and XDR connections.

# TCP
An example with a client and server using TCP connections.

# TCP select
An example with a client and server using TCP connections in which client wait server response for a while.

Function select() allows you to wait the answer (i.e coming from the sever) for a fixed time; if the answer didn't arrived in that time the program will go on.

# TCP pre-forking
An example with a client and server using TCP connections in which server satisfy client request through child process generated before accepting connection.

A varibale is defined to manage the maximum number of child process that have to be generated, then every time a connection is accepted one of these process will satisfy the request of the client.

# TCP concurrency
An example with a client and server using TCP connections in which server satisfy client request through child process generated each time a connection is accepted.

Every time a connection is accepted a child process is generated and will satisfy the request of the client. When the proces ends a SIGCHLD is triggered and through a signal handler the zombie proces will be captured.

# UDP
An example with a client and server using UDP connections.

# XDR
An example with a client and server using XDR protocol.
