#include "headers/gateway.h"
#include <stdio.h>
#include <winsock2.h>
#include "headers/controller_headers/english_controller.h"
#include "headers/controller_headers/italian_controller.h"

const size_t HEADER_BUFFER_CAPACITY = 65536;

/*
 * questo definisce una funzione generica, che verrà in futuro
 * identificata dalla parola "ControllerFn".
 * La funzione ritorna void e prende in input due parametri, un socket e un char*
 */
typedef void (*ControllerFn)(SOCKET, const char *, const char *);

// qui stiamo definendo il tipo "Route", composto da una stringa path e una funzione handler (controllerSwitch) che
// ritornerà void e che avrà l'obiettivo di switchare la chiamata al metodo del controller designato
typedef struct {
    const char *path;
    ControllerFn controllerSwitch;
} Route;

Route routes[] = {
        {"/english", englishControllerSwitch}, // stiamo legando il path "/english" alla funzione routeEnglishRequest
                                               // del controller "english_controller"
        {"/italian", italianControllerSwitch},
};

static char *read_header(SOCKET client) {
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


static char *extractHeaderInfo(const char *header) {
    char *p = strstr(header, "HTTP");
    const size_t info_len = (size_t) (p - header);
    char *headerInfo = malloc(info_len + 1);
    memcpy(headerInfo, header, info_len);
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
static char *extractHttpMethod(char *header) { return strtok(header, " "); }

static char *extractPath(char *header) { return strtok(NULL, " "); }

/*prende in input:
 * 1. il client che ha fatto la richiesta GIA' accettata dal server
 * 2. l'indirizzo del client
 * 3. la grandezza della variabile che contiene l'indirizzo
 *
 * Ha come obiettivo quello di selezionare il controller che dovrà occuparsi di questa richiesta
 */
int route_request(SOCKET client, struct sockaddr_in client_addr, int addrlen) {

    printf("-----------------------------------------------\n");

    char *header = read_header(client);

    if (header == NULL) {
        printf("Errore nella lettura dell'header\n");
        return -1;
    }
    /* stampa il contenuto ricevuto (fino agli header) per diagnosi; %.*s limita l’output ai soli
     * ‘total’ byte validi dell’accumulatore.
     */
    printf("Header ricevuto:\n%.*s", HEADER_BUFFER_CAPACITY, header);

    char *headerInfo = extractHeaderInfo(header);
    free(header);
    char *method = extractHttpMethod(headerInfo);
    char *path = extractPath(headerInfo);

    int handled = 0;
    for (int i = 0; i < sizeof(routes) / sizeof(Route); i++) {
        if (strncmp(path, routes[i].path, strlen(routes[i].path)) == 0) {
            routes[i].controllerSwitch(client, path, method);
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
             * Shutdown fa in modo che il kernel invii prima tutti i dati nel buffer
             * di invio, subito dopo invia un flag FIN al client, che dice al client
             * "Non invierò più dati". In questo modo, la connessione con il client viene
             * chiusa DOPO l'invio di tutti i dati, non MENTRE i dati vengono mandati
             */
            shutdown(client, SD_SEND);
            char tmp[128];
            recv(client, tmp, sizeof(tmp), 0); // legge eventuale ACK o close
        }
    }
    // chiude il socket di comunicazione con il client
    closesocket(client);

    // libera memoria allocata
    free(headerInfo);

    return 0;
}
