#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "tcpserver.h"

#define PATH_LENGTH 50
#define MAX_ROUTES 50

typedef void (* Handler)(char *response);

typedef struct route{
    char path[PATH_LENGTH];
    Handler handler;
} Route;

void createPostRoute(char* path, Handler handlerFunction);

void handlePost(char *response, char *request_path);

void handleGet(char *response, char *request_path);

void requestHandler(char *response, char *request_type, char *request_path);

void onConnection(int connectionFileDescriptor);

void runHTTPServer(char *port);

#endif // HTTPSERVER_H