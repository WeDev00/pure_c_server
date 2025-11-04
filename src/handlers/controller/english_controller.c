#include <stdio.h>
#include <string.h>
#include <winsock.h>

typedef void (*EndpointFn)(SOCKET client); // definizione (puntatore a) funzione che gestisce la singola richiesta

typedef struct {
    const char *method; // "GET", "POST", ecc.
    const char *path; // "/english/hello" ecc.
    EndpointFn handler; // funzione che risponde
} Endpoint;

void replyWorldRequest(SOCKET client) {}

void replyHomeRequest(SOCKET client) {}

void replyHomePostRequest(SOCKET client) {}

//Definizione endpoints
static Endpoint endpoints[] = {
        {"GET", "/english/world", replyWorldRequest},
        {"GET", "/english/home", replyHomeRequest},
        {"POST", "/english/home", replyHomePostRequest},
};


void englishControllerSwitch(const SOCKET client, const char *method, const char *path) {
    int handled = 0;
    for (int i = 0; i < sizeof(endpoints) / sizeof(Endpoint); i++) {
        if (strcmp(method, endpoints[i].method) == 0 &&
            strncmp(path, endpoints[i].path, strlen(endpoints[i].path)) == 0) {
            endpoints[i].handler(client);
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
    closesocket(client);
}
