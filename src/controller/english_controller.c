#include <stdio.h>
#include <string.h>

#include "../../headers/utility.h"

typedef void (*EndpointFn)(SOCKET client,
                           int contentLength); // definition (pointer to) function that handles the single request

typedef struct {
    const char *method; // "GET", "POST", ecc.
    const char *path; // "/english/hello" ecc.
    EndpointFn handler; // function that responds
} Endpoint;

void replyWorldRequest(SOCKET client, int contentLength) {}

void replyHomeRequest(SOCKET client, int contentLength) {}

void replyHomePostRequest(SOCKET client, int contentLength) {
    char *body = readBody(client, contentLength);

    printf("BODY LETTO:\n%.*s", contentLength, body);

    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n\r\n"
                       "Body ricevuto gg bro";
    send(client, resp, (int) strlen(resp), 0);
}

// Definizione endpoints
static Endpoint endpoints[] = {
        {"GET", "/english/world", replyWorldRequest},
        {"GET", "/english/home", replyHomeRequest},
        {"POST", "/english/home", replyHomePostRequest},
};


void englishControllerSwitch(const SOCKET client, const char *path, const char *method, int contentLenght) {
    int handled = 0;
    for (int i = 0; i < sizeof(endpoints) / sizeof(Endpoint); i++) {
        if (strcmp(method, endpoints[i].method) == 0 &&
            strncmp(path, endpoints[i].path, strlen(endpoints[i].path)) == 0) {
            endpoints[i].handler(client, contentLenght);
            handled = 1;
            break;
        }
    }

    if (!handled) {
        const char *resp = "HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n\r\n"
                           "Not Found";
        send(client, resp, (int) strlen(resp), 0);
    }
}
