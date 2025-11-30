#include "../../headers/utility/utility.h"
#include <stdio.h>
#include <stdlib.h>

const size_t HEADERS_BUFFER_CAPACITY = 65536;
const size_t BODY_BUFFER_CAPACITY = 65536;

char *readHeaders(SOCKET client) {
    char buf[4096];
    char *requestReadDataBuffer = calloc(HEADERS_BUFFER_CAPACITY, sizeof(char));
    if (!requestReadDataBuffer) {
        printf("Error: unable to allocate memory for reading headers\n");
        return NULL;
    }

    while (1) {
        int n = recv(client, buf, sizeof(buf), MSG_PEEK);

        if (n > 0) {
            if (n >= (int) HEADERS_BUFFER_CAPACITY) {
                printf("request too large (%d bytes)\n", n);
                closesocket(client);
                free(requestReadDataBuffer);
                return NULL;
            }

            memcpy(requestReadDataBuffer, buf, n);
            requestReadDataBuffer[n] = '\0';

            char *header_end = strstr(requestReadDataBuffer, "\r\n\r\n");
            if (header_end) {
                int header_len = (int) ((header_end - requestReadDataBuffer) + 4);
                char *header_only = malloc(header_len + 1);
                if (!header_only) {
                    printf("Error: malloc failed for header_only\n");
                    free(requestReadDataBuffer);
                    return NULL;
                }

                int consumed = 0;
                while (consumed < header_len) {
                    int r = recv(client, header_only + consumed, header_len - consumed, 0);
                    if (r <= 0) {
                        printf("Error while reading headers\n");
                        free(requestReadDataBuffer);
                        free(header_only);
                        return NULL;
                    }
                    consumed += r;
                }

                header_only[header_len] = '\0';
                free(requestReadDataBuffer);
                return header_only;
            }
        } else if (n == 0) {
            printf("connection closed\n");
            break;
        } else {
            int err = WSAGetLastError();
            printf("recv error: %d\n", err);
            closesocket(client);
            free(requestReadDataBuffer);
            return NULL;
        }
    }

    free(requestReadDataBuffer);
    return NULL;
}

int extractContentLenght(const char *headers) {
    const char *cl_header = strstr(headers, "Content-Length:");
    if (!cl_header) {
        printf("No Content-Length found, no body to read.\n");
        return -1;
    }

    // Extract the numeric value
    int contentLength = 0;
    if (sscanf(cl_header, "Content-Length: %d", &contentLength) != 1 || contentLength <= 0) {
        printf("Content-Length non valido.\n");
        return -1;
    }

    return contentLength;
}

char *extractHeaderInfo(const char *headers) {
    char *p = strstr(headers, "HTTP");
    const size_t info_len = (size_t) (p - headers);
    char *headerInfo = malloc(info_len + 1);
    memcpy(headerInfo, headers, info_len);
    headerInfo[info_len] = '\0';
    printf("Useful information %s\n-----------------------------------------------\n\n\n", headerInfo);
    return headerInfo;
}

/*
 * for the following 2 functions to work correctly, they must be called in order
 * of definition, this is because the method used (strtok) returns:
 * 1. On the first call, the first string found in the input parameter (the header in our case) up to the
 *    specified terminator (the space in our case)
 * 2. From the second to the n-th call, it returns the n-th occurrence in the char* passed to the FIRST call (in fact
 *    in subsequent calls it uses NULL) up to the separator
 */
char *extractHttpMethod(char *headers) { return strtok(headers, " "); }

char *extractPath(char *headers) { return strtok(NULL, " "); }

char *readBody(SOCKET client, int contentLength) {

    if (contentLength <= 0) {
        return NULL;
    }

    char *requestReadDataBuffer = calloc(BODY_BUFFER_CAPACITY + 1, sizeof(char));

    if (!requestReadDataBuffer) {
        printf("Error: unable to allocate memory for reading the body");
        return NULL;
    }

    while (1) {
        int n = recv(client, requestReadDataBuffer, contentLength, 0);

        if (n < 0) {
            int err = WSAGetLastError();
            printf("recv error: %d\n", err);
            break;
        }
        if (n == 0) {
            printf("connection closed\n");
            break;
        }
        requestReadDataBuffer[n] = '\0';
        return requestReadDataBuffer;
    }

    free(requestReadDataBuffer);
    return NULL;
}

/*
 * The following function aims to find the exact match with the endpoints for managing dynamic parameters such as
 * “{id}”; it takes the following inputs:
 * 1. the path arrived in the request
 * 2. the path defined in the endpoint
 * 3. any UUID that needs to be extractedThe function returns the path that matches the input UUID
 */
int matchEndpoint(const char *path, const char *pathPattern, char **outUUID) {
    // find the first reference to the {id} part
    const char *placeholder = strstr(pathPattern, "{id}");
    if (placeholder) { // if a reference was found
        int prefixLen = placeholder - pathPattern; // extract the part before the id

        if (strncmp(path, pathPattern, prefixLen) !=
            0) // if the arrived path doesn't match the endpoint path before the dynamic parameter
            return 0; // return false

        char *idStr = path + prefixLen; // extract the id
        if (*idStr == '\0') // if id isn't found
            return 0; // return false

        *outUUID = (char *) idStr; // save the id in the variable
        return 1; // return true
    }

    // if no dynamic parameter is found, return the classic match
    return strcmp(path, pathPattern) == 0;
}
