#include <stdio.h>
#include <winsock2.h>

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
    SOCKET s;
    struct sockaddr_in server;
    int port = 8080;

    return 0;
}
