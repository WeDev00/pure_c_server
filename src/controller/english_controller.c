#include <stdio.h>
#include <string.h>
#include "../../headers/controller/generic_controller.h"
#include "../../headers/utility/utility.h"

static void create(SOCKET client, int contentLength, char *UUID) {
    printf("/english/create");
    char *body = readBody(client, contentLength);

    printf("BODY LETTO:\n%.*s", contentLength, body);

    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n\r\n"
                       "Body ricevuto gg bro";
    send(client, resp, (int) strlen(resp), 0);
}

static void read(SOCKET client, int contentLength, char *UUID) {
    printf("/english/read/{id}");
    char resp[256];
    snprintf(resp, sizeof(resp),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Connection: close\r\n\r\n"
             "read specifica di id: %s",
             UUID);
    send(client, resp, (int) strlen(resp), 0);
}

static void readAll(SOCKET client, int contentLength, char *UUID) {
    printf("/english/readAll");
    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n\r\n"
                       "read generica";
    send(client, resp, (int) strlen(resp), 0);
}

static void update(SOCKET client, int contentLength, char *UUID) { printf("/english/update/{id}"); }

static void delete(SOCKET client, int contentLength, char *UUID) { printf("/english/delete/{id}"); }

// endpoints definitions
static Endpoint endpoints[] = {
        {"POST", "/english", create}, {"GET", "/english/{id}", read}, {"GET", "/english", readAll},
        {"PUT", "/english", update},  {"DELETE", "/english", delete},
};

void englishControllerSwitch(const SOCKET client, const char *path, const char *method, int contentLength) {
    int handled = 0;
    char *UUID = malloc(45 * sizeof(char));
    for (int i = 0; i < sizeof(endpoints) / sizeof(Endpoint); i++) {
        if (strcmp(method, endpoints[i].method) == 0 && matchEndpoint(path, endpoints[i].path, &UUID)) {
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
