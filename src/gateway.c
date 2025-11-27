#include "../headers/gateway.h"
#include <stdio.h>
#include <winsock2.h>
#include "../headers/controller/english_controller.h"
#include "../headers/controller/italian_controller.h"
#include "../headers/utility/utility.h"

/*
 * this defines a generic function, which will eventually
 * be identified by the word "ControllerFn".
 * The function returns void and takes two parameters as input, a socket and a char*
 */
typedef void (*ControllerFn)(SOCKET, const char *, const char *, int);

// here we are defining the "Route" type, composed of a string path and a handler function (controllerSwitch) that
// will return void and will have the objective of switching the call to the designated controller method
typedef struct {
    const char *path;
    ControllerFn controllerSwitch;
} Route;

Route routes[] = {
        {"/english", englishControllerSwitch}, // we are binding the path "/english" to the routeEnglishRequest function
                                               // of the "english_controller" controller

        {"/italian", italianControllerSwitch},
};

/*takes as input:
 * 1. the client that made the request ALREADY accepted by the server
 * 2. the client's address
 * 3. the size of the variable that contains the address
 *
 * Its objective is to select the controller that will handle this request
 */
int route_request(SOCKET client, struct sockaddr_in client_addr, int addrlen) {

    printf("-----------------------------------------------\n");

    char *requestHeaders = readHeaders(client);

    if (requestHeaders == NULL) {
        printf("Error reading header\n");
        return -1;
    }
    /* prints the received content (up to the headers) for diagnostics; %.*s limits the output to only
     * 'total' valid bytes of the accumulator.
     */
    printf("Header received:\n%.*s", HEADERS_BUFFER_CAPACITY, requestHeaders);

    int requestBodyContentLenght = extractContentLenght(requestHeaders);
    char *headerInfo = extractHeaderInfo(requestHeaders);
    free(requestHeaders);
    char *requestMethod = extractHttpMethod(headerInfo);
    char *requestPath = extractPath(headerInfo);

    int handled = 0;
    for (int i = 0; i < sizeof(routes) / sizeof(Route); i++) {
        if (strncmp(requestPath, routes[i].path, strlen(routes[i].path)) == 0) {
            routes[i].controllerSwitch(client, requestPath, requestMethod, requestBodyContentLenght);
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
            // continua comunque alla chiusura
        } else {
            /*
             * Shutdown ensures that the kernel sends all data in the send buffer first,
             * immediately after it sends a FIN flag to the client, which tells the client
             * "I will not send any more data". This way, the connection with the client is
             * closed AFTER all data has been sent, not WHILE the data is being sent
             */
            shutdown(client, SD_SEND);
            char tmp[128];
            recv(client, tmp, sizeof(tmp), 0); // read ACK or close if necessary
        }
    }
    // closes the communication socket with the client
    closesocket(client);

    // free allocated memory
    free(headerInfo);

    return 0;
}
