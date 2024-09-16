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

void handlePost(char *response, char *requestPath, char *requestBody)
{
    // server will look for the path at a naively implemented dictionary

    int found = 0;

    for(int i = 0; i < routeCount; i++)
    {
        if(strcmp(routes[i].path, requestPath) == 0)
        {
            found = 1;
            routes[i].handler(response, requestBody);
            return;
        }
    }
    
    if(!found)
    {
        strcpy(response, "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found");
    }
}

void handleGet(char *response, char *requestPath)
{
    // server will look for the file in the same level as the binary.

    FILE *file = fopen(requestPath + 1, "r");

    if(!file)
    {
        strcpy(response, "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found");
        return;
    }
    else
    {
        char *content_type = "";

        if(strstr(requestPath, ".html")) 
        {
            strcmp(content_type, "text/html");
        }
        else if(strstr(requestPath, ".css")) 
        {
            strcmp(content_type, "text/css");
        }
        else if(strstr(requestPath, ".js")) 
        {
            strcmp(content_type, "application/javascript");
        }
        else if(strstr(requestPath, ".png")) 
        {
            strcmp(content_type, "image/png");
        }
        else if(strstr(requestPath, ".jpeg")) 
        {
            strcmp(content_type, "image/jpeg");
        }
        else if(strstr(requestPath, ".ico"))
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

void requestHandler(char *response, char *requestType, char *requestPath, char *requestBody)
{
    if(strcmp(requestType, "POST") == 0)
    {
        handlePost(response, requestPath, requestBody);
    }
    else if(strcmp(requestType, "GET") == 0)
    {
        handleGet(response, requestPath);
    }
}

void parseRequest(char *request, char *requestType, char *requestPath, char *requestBody)
{
    sscanf(request, "%s %s", requestType, requestPath); // parses request type and request path

    if(strcmp(requestType, "POST") == 0)
    {
        char *contentLengthHeader = strstr(request, "Content-Length:"); // finds pointer to content length
        int contentLength = 0;

        if (contentLengthHeader)
        {
            sscanf(contentLengthHeader, "Content-Length: %d", &contentLength); // parses content length

            char *bodyStart = strstr(request, "\r\n\r\n"); // finds body starting point

            if (bodyStart) {
                bodyStart += 4;  // skips \r\n\r\n
                strncpy(requestBody, bodyStart, contentLength);
                requestBody[contentLength] = '\0';  
            }
        }
    }
}

void onConnection(int connectionFileDescriptor)
{
    char request[MAX_DATA_SIZE];
    char requestType[10];
    char requestPath[MAX_DATA_SIZE];
    char response[MAX_DATA_SIZE];
    char requestBody[MAX_DATA_SIZE];

    receiveString(connectionFileDescriptor, request);

    parseRequest(request, requestType, requestPath, requestBody);

    requestHandler(response, requestType, requestPath, requestBody);

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