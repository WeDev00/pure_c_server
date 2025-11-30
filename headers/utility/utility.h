#ifndef UTILITY_H
#define UTILITY_H
#include <winsock2.h>
extern const size_t HEADERS_BUFFER_CAPACITY;
extern const size_t BODY_BUFFER_CAPACITY;

typedef enum {
    NONE = 0,
    ITALIAN_ENTITY,
    ENGLISH_ENTITY,
    ITALIAN_ENTITIES,
    ENGLISH_ENTITIES,
    ERROR_MESSAGE,
} ResponseType;

char *readHeaders(SOCKET client);

int extractContentLenght(const char *headers);

char *extractHeaderInfo(const char *headers);

char *extractHttpMethod(char *headers);

char *extractPath(char *headers);

char *readBody(SOCKET client, int contentLength);

int matchEndpoint(const char *path, const char *pathPattern, char **outUUID);

void sendResponse(SOCKET client, int httpCode, ResponseType responseType, void *object);

#endif
