#include <stdio.h>
#include <string.h>
#include "../../headers/controller/generic_controller.h"
#include "../../headers/repository/english_repository.h"
#include "../../headers/service/english_service.h"
#include "../../headers/utility/utility.h"

static void create(SOCKET client, int contentLength, char *UUID) {
    printf("/english/create");
    char *body = readBody(client, contentLength);
    EnglishEntity *entity = (EnglishEntity *) jsonToObject(ENGLISH_ENTITY, body);
    const int operationResult = englishServiceCreate(*entity);
    if (operationResult == 0) {
        sendResponse(client, 200, ENGLISH_ENTITY, NULL);
    } else {
        char *errorMessage = "Qualcosa è andato storto";
        sendResponse(client, 500, ERROR_MESSAGE, errorMessage);
    }
}

static void read(SOCKET client, int contentLength, char *UUID) {
    printf("/english/read/{id}");
    EnglishEntity *entity = englishServiceRead(UUID);
    if (entity == NULL) {
        char *errorMessage = "Qualcosa è andato storto";
        sendResponse(client, 500, ERROR_MESSAGE, errorMessage);
    } else {
        sendResponse(client, 200, ENGLISH_ENTITY, entity);
    }
}

static void readAll(SOCKET client, int contentLength, char *UUID) {
    printf("/english/readAll");
    EnglishEntity **entities = englishServiceReadAll();
    if (entities == NULL)
        sendResponse(client, 500, ERROR_MESSAGE, "There's no data to read");
    else
        sendResponse(client, 200, ENGLISH_ENTITIES, entities);
}

static void update(SOCKET client, int contentLength, char *UUID) {
    printf("/english/update/{id}");
    char *body = readBody(client, contentLength);
    EnglishEntity *entity = (EnglishEntity *) jsonToObject(ENGLISH_ENTITY, body);
    const int operationResult = englishServiceUpdate(UUID, *entity);

    if (operationResult != 0) {
        char *errorMessage = "Qualcosa è andato storto";
        sendResponse(client, 500, ERROR_MESSAGE, errorMessage);
    } else {
        sendResponse(client, 200, ENGLISH_ENTITY, NULL);
    }
}

static void delete(SOCKET client, int contentLength, char *UUID) { printf("/english/delete/{id}"); }

// endpoints definitions
static Endpoint endpoints[] = {
        {"POST", "/english", create},     {"GET", "/english/{id}", read},      {"GET", "/english", readAll},
        {"PUT", "/english/{id}", update}, {"DELETE", "/english/{id}", delete},
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
