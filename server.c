#include "tcpserver.h"

#define BACKLOG 10

#define PORT "3490\0"  // the port users will be connecting to

void onConnection(int connectionFileDescriptor)
{
    sendString(connectionFileDescriptor, "teste");
}

int main(int argc, char **argv)
{
    TCPServer server;

    runServer(&server, PORT);    

    while(1)
    {
        acceptConnection(&server);
        
        int connectionFileDescriptor = server.newConnectionFileDescriptor;

        if(!fork())
        {
            close(server.socketFileDescriptor);

            onConnection(connectionFileDescriptor);
            
            close(connectionFileDescriptor);
            exit(0);
        }   
        
        close(connectionFileDescriptor);
    }

}
