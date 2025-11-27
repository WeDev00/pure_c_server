#include <stdio.h>
#include <string.h>
#include "../../headers/controller/generic_controller.h"


static void create(SOCKET client, int contentLength, int UUID) {
    char *body = readBody(client, contentLength);

    printf("READ BODY:\n%.*s", contentLength, body);

    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n\r\n"
                       "Body received gg bro";
    send(client, resp, (int) strlen(resp), 0);
}

static void read(SOCKET client, int contentLength, int UUID) {}

static void readAll(SOCKET client, int contentLength, int UUID) {}

static void update(SOCKET client, int contentLength, int UUID) {}

static void delete(SOCKET client, int contentLength, int UUID) {}

static Endpoint endpoints[] = {
        {"POST", "/italian", create}, {"GET", "/italian", readAll},   {"GET", "/italian/{id}", read},
        {"PUT", "/italian", update},  {"DELETE", "/italian", delete},
};

void italianControllerSwitch(const SOCKET client, const char *path, const char *method, int contentLength) {

    int handled = 0;
    int UUID = 0;
    for (int i = 0; i < sizeof(endpoints) / sizeof(Endpoint); i++) {
        if (strncmp(path, endpoints[i].path, strlen(endpoints[i].path)) == 0 &&
            matchEndpoint(path, endpoints[i].path, &UUID)) {
            endpoints[i].handler(client, contentLength, UUID);
            handled = 1;
            break;
        }
    }

    if (handled == 0) {
        const char *resp = "HTTP/1.1 404 NOT_FOUND\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "NOT_FOUND";
        if (send(client, resp, (int) strlen(resp), 0) == SOCKET_ERROR) {
            int err = WSAGetLastError();
            printf("send error: %d\n", err);
        }
    }
}
