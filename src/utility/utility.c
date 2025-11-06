#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

char *read_headers(SOCKET client) {
    char buf[4096];
    char *requestReadDataBuffer = calloc(HEADER_BUFFER_CAPACITY, sizeof(char));
    if (!requestReadDataBuffer) {
        printf("Errore: impossibile allocare memoria per gli header\n");
        return NULL;
    }

    while (1) {
        int n = recv(client, buf, sizeof(buf), MSG_PEEK);

        if (n > 0) {
            if (n >= (int) HEADER_BUFFER_CAPACITY) {
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
                    printf("Errore: malloc fallita per header_only\n");
                    free(requestReadDataBuffer);
                    return NULL;
                }

                int consumed = 0;
                while (consumed < header_len) {
                    int r = recv(client, header_only + consumed, header_len - consumed, 0);
                    if (r <= 0) {
                        printf("Errore durante la lettura effettiva degli header\n");
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
        printf("Nessun Content-Length trovato, nessun body da leggere.\n");
        return -1;
    }

    // 2️⃣ Estrarre il valore numerico
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
    printf("Info utili %s\n-----------------------------------------------\n\n\n", headerInfo);
    return headerInfo;
}

/*
 * affinchè le seguenti 2 funzioni funzionino correttamente, devono essere chiamate in ordine
 * di definizione, questo perchè il metodo usato (strtok) ritorna:
 * 1. Alla prima chiamata, la prima stringa trovata nel parametro in input (l'header nel nostro caso) fino al
 *    terminatore specificato (lo spazio nel nostro caso)
 * 2. Dalla seconda all'n-esima chiamata, ritorna l'n-esima occorrenza nel char* passato alla PRIMA chiamata (infatti
 *    nelle successive usa NULL) fino al separatore
 */
char *extractHttpMethod(char *headers) { return strtok(headers, " "); }

char *extractPath(char *headers) { return strtok(NULL, " "); }
