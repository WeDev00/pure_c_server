#include "headers/gateway.h"
#include <stdio.h>
#include <winsock2.h>
#include "../utility/utility.h"
#include "headers/controller_headers/english_controller.h"
#include "headers/controller_headers/italian_controller.h"


/*
 * questo definisce una funzione generica, che verrà in futuro
 * identificata dalla parola "ControllerFn".
 * La funzione ritorna void e prende in input due parametri, un socket e un char*
 */
typedef void (*ControllerFn)(SOCKET, const char *, const char *, int);

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


/*prende in input:
 * 1. il client che ha fatto la richiesta GIA' accettata dal server
 * 2. l'indirizzo del client
 * 3. la grandezza della variabile che contiene l'indirizzo
 *
 * Ha come obiettivo quello di selezionare il controller che dovrà occuparsi di questa richiesta
 */
int route_request(SOCKET client, struct sockaddr_in client_addr, int addrlen) {

    printf("-----------------------------------------------\n");

    char *header = read_headers(client);

    if (header == NULL) {
        printf("Errore nella lettura dell'header\n");
        return -1;
    }
    /* stampa il contenuto ricevuto (fino agli header) per diagnosi; %.*s limita l’output ai soli
     * ‘total’ byte validi dell’accumulatore.
     */
    printf("Header ricevuto:\n%.*s", HEADER_BUFFER_CAPACITY, header);

    int contentLenght = extractContentLenght(header);
    char *headerInfo = extractHeaderInfo(header);
    free(header);
    char *method = extractHttpMethod(headerInfo);
    char *path = extractPath(headerInfo);

    int handled = 0;
    for (int i = 0; i < sizeof(routes) / sizeof(Route); i++) {
        if (strncmp(path, routes[i].path, strlen(routes[i].path)) == 0) {
            routes[i].controllerSwitch(client, path, method, contentLenght);
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
