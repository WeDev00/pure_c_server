#include "headers/requests_handler.h"
#include <stdio.h>
#include <winsock2.h>

size_t HEADER_BUFFER_CAPACITY = 65536;
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


/*prende in input:
 * 1. il client che ha fatto la richiesta GIA' accettata dal server
 * 2. l'indirizzo del client
 * 3. la grandezza della variabile che contiene l'indirizzo
 */
int handle_request(SOCKET client, struct sockaddr_in client_addr, int addrlen) {

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
    char *info = malloc(info_len + 1);
    memcpy(info, header, info_len);
    info[info_len] = '\0';
    printf("Info utili %s\n-----------------------------------------------\n\n\n", info);

    free(header);
    free(p);
    free(info);

    // 4) Risponde con un 200 minimale e chiude
    const char *resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Content-Length: 2\r\n"
                       "Connection: close\r\n"
                       "\r\n"
                       "OK";
    if (send(client, resp, (int) strlen(resp), 0) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        printf("send error: %d\n", err);
        // continua comunque alla chiusura
    }

    // 5) Chiude la connessione del client
    closesocket(client);
    return 0;
}
