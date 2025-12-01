#include <stdio.h>
#include <string.h>
#include "../../headers/controller/generic_controller.h"
#include "../../headers/repository/italian_repository.h"
#include "../../headers/service/italian_service.h"


static void create(SOCKET client, int contentLength, char *UUID) {
    printf("italian/create");
    char *body = readBody(client, contentLength);

    printf("READ BODY:\n%.*s", contentLength, body);

    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n\r\n"
                       "Body received gg bro";
    send(client, resp, (int) strlen(resp), 0);
}

static void read(SOCKET client, int contentLength, char *UUID) {
    printf("italian/read/{id}");
    const ItalianEntity *entity = italianServiceRead(UUID);
    sendResponse(client, 200, ITALIAN_ENTITY, entity);
}

static void readAll(SOCKET client, int contentLength, char *UUID) {
    printf("italian/readAll");
    ItalianEntity **entities = italianServiceReadAll();
    sendResponse(client, 200, ITALIAN_ENTITIES, entities);
}

static void update(SOCKET client, int contentLength, char *UUID) { printf("italian/update/{id}"); }

static void delete(SOCKET client, int contentLength, char *UUID) { printf("italian/delete/{id}"); }

static Endpoint endpoints[] = {
        {"POST", "/italian", create}, {"GET", "/italian", readAll},   {"GET", "/italian/{id}", read},
        {"PUT", "/italian", update},  {"DELETE", "/italian", delete},
};

void italianControllerSwitch(const SOCKET client, const char *path, const char *method, int contentLength) {

    int handled = 0;
    char *UUID = malloc(45 * sizeof(char));
    for (int i = 0; i < sizeof(endpoints) / sizeof(Endpoint); i++) {

        const char *brace_pos = strchr(endpoints[i].path, '{');
        size_t cmp_len;
        if (brace_pos != NULL) {
            cmp_len = (size_t) (brace_pos - endpoints[i].path) - 1;
        } else {
            cmp_len = strlen(endpoints[i].path);
        }
        const int isPathValid = strncmp(path, endpoints[i].path, cmp_len);
        if (isPathValid == 0 && strcmp(method, endpoints[i].method) == 0 &&
            matchEndpoint(path, endpoints[i].path, &UUID)) {
            endpoints[i].handler(client, contentLength, UUID);
            handled = 1;
            break;
        }
    }

    if (handled == 0) {
        char *errorMessage = "Method not found";
        sendResponse(client, 400, ERROR_MESSAGE, errorMessage);
    }
}
