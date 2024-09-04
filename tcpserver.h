#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_DATA_SIZE 2048 // max number of bytes we can get at once 
#define HOST_SIZE 15
#define PORT_SIZE 5
#define BACKLOG 10

typedef struct TCPServer{
    char port[PORT_SIZE];
    int socketFileDescriptor;
    int newConnectionFileDescriptor;
    struct sockaddr_storage addressStorage;
    
} TCPServer;

void runServer(TCPServer *server, char *port);

struct addrinfo *getServerInformation(TCPServer *server);

void bindAddress(TCPServer *server);

void listenForConnections(TCPServer *server);

void acceptConnection(TCPServer *server);

void sendString(int connectionFileDescriptor, char *response);

void receiveString(int connectionFileDescriptor, char *string);

void killDeadProcesses();

void sigchld_handler(int s);

void *get_in_addr(struct sockaddr *socketAddress);