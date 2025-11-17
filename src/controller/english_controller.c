#include <stdio.h>
#include <string.h>
#include "../../headers/controller_headers/generic_controller.h"
#include "../../headers/utility.h"

void create(SOCKET client, int contentLength, int UUID) {
    char *body = readBody(client, contentLength);

    printf("BODY LETTO:\n%.*s", contentLength, body);

    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n\r\n"
                       "Body ricevuto gg bro";
    send(client, resp, (int) strlen(resp), 0);
}

void read(SOCKET client, int contentLength, int UUID) {}

void update(SOCKET client, int contentLength, int UUID) {}

void delete(SOCKET client, int contentLength, int UUID) {}

// endpoints definitions
static Endpoint endpoints[] = {
        {"POST", "/english", create},
        {"GET", "/english", read},
        {"PUT", "/english", update},
        {"DELETE", "/english", delete},
};


void englishControllerSwitch(const SOCKET client, const char *path, const char *method, int contentLength) {
    int handled = 0;
    int UUID = 0; // TO-DO: replace with extractUUID
    for (int i = 0; i < sizeof(endpoints) / sizeof(Endpoint); i++) {
        if (strcmp(method, endpoints[i].method) == 0 &&
            strncmp(path, endpoints[i].path, strlen(endpoints[i].path)) == 0) {
            endpoints[i].handler(client, contentLength, UUID);
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
