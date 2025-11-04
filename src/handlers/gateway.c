#include "headers/gateway.h"
#include <stdio.h>
#include <winsock2.h>

#include "../../../../../../../Program Files/JetBrains/CLion 2025.2.3/bin/mingw/lib/gcc/x86_64-w64-mingw32/13.1.0/include/stdbool.h"
#include "headers/controller_headers/english_controller.h"
#include "headers/controller_headers/italian_controller.h"

const size_t HEADER_BUFFER_CAPACITY = 65536;

/*
 * questo definisce una funzione generica, che verrà in futuro
 * identificata dalla parola "ControllerFn".
 * La funzione ritorna void e prende in input due parametri, un socket e un char*
 */
typedef void (*ControllerFn)(SOCKET, char *, char *);

// qui stiamo definendo il tipo "Route", composto da una stringa path e una funzione handler che ritornerà void
typedef struct {
    const char *path;
    ControllerFn handler;
} Route;

Route routes[] = {
        {"/english", englishControllerSwitch}, // stiamo legando il path "/english" alla funzione routeEnglishRequest
                                               // del controller "english_controller"
        {"/italian", italianControllerSwitch},
};

static char *read_header(SOCKET client) {

    /*
     * buffer temporaneo dove verranno memorizzati i byte INCREMENTALI letti dalla richiesta
     * (4 kylobyte è una grandezza tipica per chunk tcp)
     */
    char buf[4096];

    /*
     * buffer dove verranno memorizzati TUTTI i dati della richiesta http
     * HEADER_BUFFER_CAPACITY = 65 KiB è una grandezza necessaria a leggere
     * solo gli header; viene inizializzato con gli zero per garantire la terminazione di stringa
     */
    char *requestReadDataBuffer = calloc(HEADER_BUFFER_CAPACITY, sizeof(char));

    // variabile usata per accumulare i byte letti dalla richiesta
    int total = 0;

    while (1) {
        /*
         * recv legge {sizeof(buf)} = 4KiB di dati dalla richiesta e li accumula in
         * buf, l'ultimo parametro è 'flag' e serve a modificare il comportamento della recv;
         * 0 significa nessun comportamento speciale, quindi legge i dati e li rimuove
         * dal buffer che contiene i dati della richiesta.
         * ritorna un intero che se:
         * 1. maggiore di zero -> byte effettivamente letti -> la lettura è andata a buon fine
         * 2. uguale a zero -> chiusura ordinata dal client
         * 3. minore di zero -> errore nella lettura
         */
        int n = recv(client, buf, sizeof(buf), 0);

        // se la lettura è andata bene
        if (n > 0) {
            // se i byte letti finora + quelli appena letti eccedono la capacità del buffer di accumulazione
            if (total + n >= (int) HEADER_BUFFER_CAPACITY) {
                // protezione: richiesta troppo grande
                printf("request too large\n");
                printf("Byte letti finora %s\n", requestReadDataBuffer);
                closesocket(client);
                free(requestReadDataBuffer);
                return NULL;
            }

            // copia  il chunk appena letto(presente in buf) in coda a  requestReadDataBuffer, partendo
            // dal byte numero {total}
            memcpy(requestReadDataBuffer + total, buf, n);
            total += n;
            requestReadDataBuffer[total] = '\0';

            /*
             * se il terminatore "\r\n\r\n" (due caratteri CRLF, cioè "Carriage Return Line Feed"
             * storicamente [nel contesto delle macchine da scrivere] indicano
             * "Riporta il carrello all'inizio della riga" e "Fai scendere il rullo di una riga in basso")
             * che indica la fine dell'header viene trovato, allora usciamo dal ciclo di lettura
             */
            if (strstr(requestReadDataBuffer, "\r\n\r\n"))
                break;
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

    return requestReadDataBuffer;
}

static char *extractHttpMethod(char *headerInfo) { return strtok(headerInfo, " "); }

static char *extractPath(char *headerInfo) { return strtok(NULL, " "); }

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

    char *p = strstr(header, "HTTP");
    size_t info_len = (size_t) (p - header);
    char *headerInfo = malloc(info_len + 1);
    memcpy(headerInfo, header, info_len);
    headerInfo[info_len] = '\0';
    printf("Info utili %s\n-----------------------------------------------\n\n\n", headerInfo);
    free(header);
    char *method = extractHttpMethod(headerInfo);
    char *path = extractPath(headerInfo);

    int handled = 0;
    for (int i = 0; i < sizeof(routes) / sizeof(Route); i++) {
        if (strncmp(path, routes[i].path, strlen(routes[i].path)) == 0) {
            routes[i].handler(client, path, method);
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
        }
    }
    // chiude il socket di comunicazione con il client
    closesocket(client);

    // libera memoria allocata
    free(headerInfo);

    return 0;
}
