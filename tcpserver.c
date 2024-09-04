#include "tcpserver.h"

void runServer(TCPServer *server, char *port)
{
    strcpy(server->port, port);

    bindAddress(server);

    listenForConnections(server);

    killDeadProcesses();

    return;
}

void bindAddress(TCPServer *server)
{
    struct addrinfo hints, *serverInformation, *result;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    int rv = 0;
    if ((rv = getaddrinfo(NULL, server->port, &hints, &serverInformation)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }
    
    for(result = serverInformation; result != NULL; result = result->ai_next) 
    {
        if ((server->socketFileDescriptor = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) 
        {
            perror("socket()");
            continue;
        }

        int yes = 1;
        if (setsockopt(server->socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
        {
            perror("setsockopt()");
            exit(1);
        }

        if (bind(server->socketFileDescriptor, result->ai_addr, result->ai_addrlen) == -1) 
        {
            close(server->socketFileDescriptor);
            perror("bind()");
            continue;
        }

        break;
    }

    freeaddrinfo(serverInformation);

    if (result == NULL)  {
        fprintf(stderr, "bind() failed \n");
        exit(1);
    }
}

void acceptConnection(TCPServer *server)
{
    socklen_t storageSize = sizeof server->addressStorage;
    server->newConnectionFileDescriptor = accept(server->socketFileDescriptor, (struct sockaddr *)&(server->addressStorage), &storageSize);
    if (server->newConnectionFileDescriptor == -1) {
        perror("accept");
        return;
    }

    char address[INET6_ADDRSTRLEN];
    inet_ntop(server->addressStorage.ss_family,
        get_in_addr((struct sockaddr *)&(server->addressStorage)),
        address, sizeof address);
    printf("server: got connection from %s\n", address);
}

void listenForConnections(TCPServer *server)
{
    if (listen(server->socketFileDescriptor, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    printf("Server listening on port %s\n", server->port);
}

void killDeadProcesses()
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

void sendString(int connectionFileDescriptor, char *string)
{
    if(send(connectionFileDescriptor, string, strlen(string), 0) == -1)
    {
        perror("send()");
    }
}

void receiveString(int connectionFileDescriptor, char *string)
{
    if(recv(connectionFileDescriptor, string, MAX_DATA_SIZE, 0) == -1)
    {
        perror("recv()");
    }

    printf("request -> %s", string);
}

void sigchld_handler(int s)
{
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *socketAddress)
{
    if (socketAddress->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)socketAddress)->sin_addr);
    }

    return &(((struct sockaddr_in6*)socketAddress)->sin6_addr);
}