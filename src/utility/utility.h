#ifndef UTILITY_H
#define UTILITY_H
#include <winsock2.h>
const size_t HEADER_BUFFER_CAPACITY = 65536;

char *read_headers(SOCKET client);

int extractContentLenght(const char *headers);

char *extractHeaderInfo(const char *headers);

char *extractHttpMethod(char *headers);

char *extractPath(char *headers);

#endif
