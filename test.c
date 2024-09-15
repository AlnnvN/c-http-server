#include "httpserver.h"

#define PORT "3490\0" 

void helloWorldHandler(char *response)
{
    char body[] = "{'information': 'Hello, World!','data': 'This is a test message'}";
    snprintf(response, MAX_DATA_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %li\r\n\r\n%s", strlen(body), body);
}

void exampleHandler(char *response)
{
    char body[] = "{'data': 'Example'}";
    snprintf(response, MAX_DATA_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %li\r\n\r\n%s", strlen(body), body);
}

int main(int argc, char **argv)
{
    createPostRoute("/helloworld", helloWorldHandler);

    createPostRoute("/example", exampleHandler);

    runHTTPServer(PORT);
}
