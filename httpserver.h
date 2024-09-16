#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "tcpserver.h"

#define PATH_LENGTH 50
#define MAX_ROUTES 50

typedef void (* Handler)(char *response, char *body);

typedef struct route{
    char path[PATH_LENGTH];
    Handler handler;
} Route;

void createPostRoute(char* path, Handler handlerFunction);

void handlePost(char *response, char *requestPath, char *requestBody);

void handleGet(char *response, char *requestPath);

void parseRequest(char *request, char *requestType, char *requestPath, char *requestBody);

void requestHandler(char *response, char *requestType, char *requestPath, char *requestBody);

void onConnection(int connectionFileDescriptor);

void runHTTPServer(char *port);

#endif // HTTPSERVER_H