#ifndef GENERIC_CONTROLLER_H
#define GENERIC_CONTROLLER_H

#include "../../headers/utility.h" // for SOCKET

typedef void (*EndpointFn)(SOCKET client, int contentLength, int UUID);

typedef struct {
    const char *method; // "GET", "POST", ecc.
    const char *path; // "/english/hello" ecc.
    EndpointFn handler; // funzione che gestisce la richiesta
} Endpoint;

#endif
