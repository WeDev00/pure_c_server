#include <stdio.h>
#include <winsock2.h>
#include "handlers/headers/requests_handler.h"


/**
 * questa è la funzione principale che deve inizializzare il socket
 * usando le API di windows per l'utilizzo delle reti di sistema
 *
 * Essendo questo un 'server' che girerà su windows 11, dobbiamo usare winsock. Fosse stato un
 * server su linux, avremmo dovuto usare API posix
 * @return 0
 */
int main(void) {
    /*
    WSDATA viene da winsock ed è una struttura dati che racchiude diverse informazioni
    sull'implementazione di winsock attualmente in uso.
    Fin quando questa variabile non verrà popolata, non sarà possibile usare nessuna funzione di networking di windows
    */
    WSADATA wsa;

    /*
     * SOCKET è un typedef che rappresenta un descrittore di un socket
     * (cioè un canale di comunicazione di rete via TCP/IP).
     * di fatto è un numero che la libreria di windows usa per
     * identificare la struttura che gestisce le connessioni di rete
     */
    SOCKET socketReference;

    // Struttura che contiene le informazioni di indirizzo e porta del server.
    struct sockaddr_in server;
    // indica semplicemente il port scelto
    int port = 8080;

    /*
     * la funzione WSAStartup inizializza la libreria e popola la variabile wsa.
     * Se questa funzione incontra qualche errore, ritorna un valore diverso da zero
     */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }


    /*
     * la funzione socket() crea (a livello di kernel) un nuovo socket TCP usando IpV4.
     * se incontra qualche errore, ritorna il valore costante INVALID_SOCKET.
     * I parametri che accetta sono:
     * 1. af (Address Family) : AF:_INET indica ipv4, mentre AF_INET6 indica ipv6
     * 2. type: specifica il tipo di comunicazione SOCK_STREAM → Connessione (TCP): canale affidabile e continuo tra due
     * endpoint. SOCK_DGRAM → Datagrammi (UDP): consente l’invio di pacchetti singoli. 3.protocol: Indica il protocollo
     * specifico da usare: IPPROTO_TCP → Usa il protocollo TCP IPPROTO_UDP → Usa UDP 0 → Se il valore è 0, il sistema
     * operativo sceglierà automaticamente il protocollo “default” della combinazione af e type dati.
     */
    if ((socketReference = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }


    /*
     * Prepara la struttura sockaddr_in, quindi memorizziamo le decisioni che abbiamo preso nella
     * chiamata a "socket()" nella struttura server
     * sin_family indica "Socket INternal family, indica la famiglia di indirizzi (in questo caso IPV4)
     * sin_address indica l'indirizzo IP che avrà il server
     * sin_port indica il port su cui il server attenderà le chiamate http
     */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr =
            inet_addr("127.0.0.1"); // indirizzo raggiungibile solo in locale, dalla stessa macchina su cui viene
                                    // avviato il server, in altri casi bisognerebbe mettere un indirizzo ip
                                    // appartenente alla macchina che fa girare il server
    server.sin_port = htons(port);


    /*
     * la funzione bind è fondamentale per legare le informazioni che abbiamo messo in "server"
     * al riferimento al socket che abbiamo creato; ha bisogno di:
     * 1. il riferimento al socket
     * 2. il puntatore all'area di memoria dove sono storate le info sul server
     * 3. dimensioni della struttura che contiene le info sul server
     *
     * Dopo questa operazione il socket smette di essere solo una "porta" potenziale e diventa
     * una vera e propria porta legata ad un indirizzo ip, quindi adesso può funzionare correttamente
     */
    if (bind(socketReference, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        closesocket(socketReference);
        WSACleanup();
        return 1;
    }

    printf("Server in ascolto sulla porta %d all'indirizzo %s \n", ntohs(server.sin_port), inet_ntoa(server.sin_addr));


    /*
     * imposta il socket in listen, dove 3 è la grandezza della queue che può contenere delle richieste in attesa
     * che quelle in elaborazione finiscano.
     * Cambia lo stato del socket, marcandolo come “passivo” (non inizia connessioni, ma le accetta).
     * Costruisce una coda di richieste di connessione: le connessioni in arrivo vengono accodate, e il server può poi
     * accettarle una alla volta con accept. Non accetta ancora alcun client: mette solo il server “pronto ad
     * ascoltare”.
     */
    listen(socketReference, 3);

    // variabile dello stesso tipo di 'server', contiene informazioni tipo indirizzo ip e port del client che farà la
    // richiesta
    struct sockaddr_in client;
    // memorizza la grandezza della variabile client, in termini di byte
    int client_len = sizeof(client);
    int requestsCount = 0;
    while (1) {
        /*
         la funzione "accept" serve ad accettare la prima richiesta presente nella coda di ascolto
         Crea un nuovo socket che verrà usato per la comunicazione con il client; Ritorna il numero usato da windows per
         identificare questo canale di comunicazione a livello hardware.
         I parametri accettati sono:
         1. socketReference = riferimento di windows al socket del server da cui estrarre la richiesta
            (in altre parole, la coda da cui estrarre la richiesta)
         2. il puntatore all'area di memoria dedicata al client, che verrà riempita con le informazioni del tipo
         socketaddr_in
         3. la dimensione dell'area di memoria dedicata, che verrà eventualmente manipolata per essere riallocata in
         caso di spazio insufficiente

         In caso di errore ritorna la costante INVALID_SOCKET
         */
        SOCKET client_socket = accept(socketReference, (struct sockaddr *) &client, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Errore nell'accettazione della connessione.\n");
            closesocket(socketReference);
            WSACleanup();
            return 1;
        }
        /*
         In caso di connessione andata a buon fine, stampiamo:
         1.L'indirizzo ip del client, convertito in stringa da inet_ntoa
         2.Il port usato dal client convertito in numero da ntohs
         */
        printf("Connessione accettata da %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        requestsCount++;

        handle_request(client_socket, client, client_len);

        // chiude il socket
        closesocket(client_socket); // Chiude subito la connessione, senza leggere nulla

        if (requestsCount == 20)
            break;
    }

    printf("Digita INVIO per chiudere il server\n");
    // funzione che aspetta un input da tastiera, fino a quando non lo riceve "blocca" l'esecuzione, simulando
    // l'accensione del server e la messa in attesa
    getchar();


    closesocket(socketReference);
    // termina la libreria e svuota sockaddr_in
    WSACleanup();
    return 0;
}
