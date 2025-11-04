#include <stdio.h>
#include <string.h>
#include <winsock.h>

typedef void (*EndpointFn)(SOCKET client);

typedef struct {
    const char *method;
    const char *path;
    EndpointFn endpoint;
} Endpoint;

void replyMondoRequest(SOCKET client) {
    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n"
                       "\r\n"
                       "Ciao Mondo";
    if (send(client, resp, (int) strlen(resp), 0) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        printf("send error: %d\n", err);
    }
}

void replyCasaRequest(SOCKET client) {
    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Connection: close\r\n"
                       "\r\n"
                       "Ciao Casa";
    if (send(client, resp, (int) strlen(resp), 0) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        printf("send error: %d\n", err);
    }
}

Endpoint endpoints[] = {
        {"GET", "/italian/mondo", replyMondoRequest},
        {"GET", "/italian/casa", replyCasaRequest},
};

void italianControllerSwitch(const SOCKET client, const char *path, const char *method) {

    int handled = 0;

    for (int i = 0; i < sizeof(endpoints) / sizeof(Endpoint); i++) {
        if (strncmp(path, endpoints[i].path, strlen(endpoints[i].path)) == 0 &&
            strncmp(method, endpoints[i].method, strlen(endpoints[i].method)) == 0) {
            endpoints[i].endpoint(client);
            handled = 1;
            break;
        }
    }

    if (handled == 0) {
        const char *resp = "HTTP/1.1 400 BAD_REQUEST\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "BAD_REQUEST";
        if (send(client, resp, (int) strlen(resp), 0) == SOCKET_ERROR) {
            int err = WSAGetLastError();
            printf("send error: %d\n", err);
        }
    }
    closesocket(client);
}
