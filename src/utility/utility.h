#ifndef UTILITY_H
#define UTILITY_H
#include <winsock2.h>
extern const size_t HEADERS_BUFFER_CAPACITY;
extern const size_t BODY_BUFFER_CAPACITY;

char *readHeaders(SOCKET client);

int extractContentLenght(const char *headers);

char *extractHeaderInfo(const char *headers);

char *extractHttpMethod(char *headers);

char *extractPath(char *headers);

char *readBody(SOCKET client, int contentLength);

#endif
