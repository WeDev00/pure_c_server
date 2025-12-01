#include <stdio.h>
#include <string.h>
#include "../../headers/controller/generic_controller.h"
#include "../../headers/repository/italian_repository.h"
#include "../../headers/service/italian_service.h"


static void create(SOCKET client, int contentLength, char *UUID) {
    printf("italian/create");
    char *body = readBody(client, contentLength);
    ItalianEntity *entity = (ItalianEntity *) jsonToObject(ITALIAN_ENTITY, body);
    const int operationResult = italianServiceCreate(*entity);
    if (operationResult == 0) {
        sendResponse(client, 200, ITALIAN_ENTITY, NULL);
    } else {
        char *errorMessage = "Qualcosa è andato storto";
        sendResponse(client, 500, ERROR_MESSAGE, errorMessage);
    }
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

static void update(SOCKET client, int contentLength, char *UUID) {
    printf("italian/update/{id}");
    char *body = readBody(client, contentLength);
    ItalianEntity *entity = (ItalianEntity *) jsonToObject(ITALIAN_ENTITY, body);
    const int operationResult = italianServiceUpdate(UUID, *entity);

    if (operationResult != 0) {
        char *errorMessage = "Qualcosa è andato storto";
        sendResponse(client, 500, ERROR_MESSAGE, errorMessage);
    } else {
        sendResponse(client, 200, ITALIAN_ENTITY, NULL);
    }
}

static void delete(SOCKET client, int contentLength, char *UUID) {
    printf("italian/delete/{id}");
    const int operationResult = italianServiceDelete(UUID);
    if (operationResult <= 0) {
        char *errorMessage = "Qualcosa è andato storto";
        sendResponse(client, 500, ERROR_MESSAGE, errorMessage);
    } else {
        sendResponse(client, 200, ITALIAN_ENTITY, NULL);
    }
}

static Endpoint endpoints[] = {
        {"POST", "/italian", create},     {"GET", "/italian", readAll},        {"GET", "/italian/{id}", read},
        {"PUT", "/italian/{id}", update}, {"DELETE", "/italian/{id}", delete},
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
