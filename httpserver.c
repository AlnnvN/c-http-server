#include "httpserver.h"

Route routes[MAX_ROUTES]; 
int routeCount = 0;

void createPostRoute(char *path, Handler handlerFunction)
{
    if (routeCount >= MAX_ROUTES) {
        printf("Error: Maximum number of routes reached.\n");
        return;
    }

    strcpy(routes[routeCount].path, path);
    routes[routeCount].handler = handlerFunction;
    routeCount++;
}

void handlePost(char *response, char *request_path)
{
    // server will look for the path at a naively implemented dictionary

    int found = 0;

    for(int i = 0; i < routeCount; i++)
    {
        if(strcmp(routes[i].path, request_path) == 0)
        {
            found = 1;
            routes[i].handler(response);
            return;
        }
    }
    
    if(!found)
    {
        strcpy(response, "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found");
    }
}

void handleGet(char *response, char *request_path)
{
    // server will look for the file in the same level as the binary.

    FILE *file = fopen(request_path + 1, "r");

    if(!file)
    {
        strcpy(response, "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found");
        return;
    }
    else
    {
        char *content_type = "";

        if(strstr(request_path, ".html")) 
        {
            strcmp(content_type, "text/html");
        }
        else if(strstr(request_path, ".css")) 
        {
            strcmp(content_type, "text/css");
        }
        else if(strstr(request_path, ".js")) 
        {
            strcmp(content_type, "application/javascript");
        }
        else if(strstr(request_path, ".png")) 
        {
            strcmp(content_type, "image/png");
        }
        else if(strstr(request_path, ".jpeg")) 
        {
            strcmp(content_type, "image/jpeg");
        }
        else if(strstr(request_path, ".ico"))
        {
            strcmp(content_type, "image/x-icon");
        }

        snprintf(response, MAX_DATA_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);

        int header_length = strlen(response);

        int bytes_read;
        while ((bytes_read = fread(response + header_length, 1, MAX_DATA_SIZE - header_length - 1, file)) > 0) {
            header_length += bytes_read;
            response[header_length] = '\0';
        }

        fclose(file);
    }
}

void requestHandler(char *response, char *request_type, char *request_path)
{
    if(strcmp(request_type, "POST") == 0)
    {
        handlePost(response, request_path);
    }
    else if(strcmp(request_type, "GET") == 0)
    {
        handleGet(response, request_path);
    }
}

void onConnection(int connectionFileDescriptor)
{
    char request[MAX_DATA_SIZE];
    char request_type[10];
    char request_path[MAX_DATA_SIZE];
    char response[MAX_DATA_SIZE];

    receiveString(connectionFileDescriptor, request);

    sscanf(request, "%s %s", request_type, request_path); // parses request type and request path

    requestHandler(response, request_type, request_path);

    sendString(connectionFileDescriptor, response);
}

void runHTTPServer(char *port)
{
    TCPServer server;

    runServer(&server, port);    

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