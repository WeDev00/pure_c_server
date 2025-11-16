#include <stdio.h>
#include <winsock2.h>
#include "../headers/db/db_connection.h"
#include "../headers/gateway.h"

/**
 * this is the main function that must initialize the socket
 * using the Windows APIs for system network usage
 *
 * Since this is a 'server' that will run on Windows 11, we need to use winsock. Had it been a
 * server on Linux, we would have had to use POSIX APIs
 * @return 0
 */
int main(void) {
    /*
     * WSADATA comes from winsock and is a data structure that contains various information
     * about the winsock implementation currently in use.
     * Until this variable is populated, it will not be possible to use any
     * Windows networking function
     */
    WSADATA wsa;

    /*
     * SOCKET is a typedef that represents a socket descriptor
     * (that is, a network communication channel via TCP/IP).
     * in fact, it is a number that the Windows library uses to
     * identify the structure that manages network connections
     */
    SOCKET socketReference;

    // Structure containing the server address and port information.
    struct sockaddr_in server;
    // simply indicates the selected port
    int port = 8080;


    // variable containing connection information to the local database server
    const char *conninfo =
            "host=localhost port=5432 dbname=pure_c_server_db user=pure_c_server_db password=pure_c_server_db";
    // We connect to the database and verify that the connection was successful.
    DBConnection *db = db_connect(conninfo);
    if (!db) {
        fprintf(stderr, "Unable to connect to the database: abort.\n");
        return 1;
    }

    /*
     * the WSAStartup function initializes the library and populates the wsa variable.
     * If this function encounters any error, it returns a non-zero value
     */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    /*
     * the socket() function creates (at kernel level) a new TCP socket using IPv4.
     * if it encounters any error, it returns the constant value INVALID_SOCKET.
     * The parameters it accepts are:
     * 1. af (Address Family): AF_INET indicates IPv4, while AF_INET6 indicates IPv6
     * 2. type: specifies the type of communication SOCK_STREAM → Connection (TCP): reliable and continuous channel
     * between two endpoints. SOCK_DGRAM → Datagrams (UDP): allows sending individual packets. 3. protocol: Indicates
     * the specific protocol to use: IPPROTO_TCP → Uses the TCP protocol IPPROTO_UDP → Uses UDP 0 → If the value is 0,
     * the operating system will automatically choose the "default" protocol for the given af and type combination.
     */
    if ((socketReference = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }

    /*
     * Prepares the sockaddr_in structure, so we store the decisions we made in the
     * call to "socket()" in the server structure
     * sin_family indicates "Socket INternal family", indicates the address family (in this case IPv4)
     * sin_addr indicates the IP address that the server will have
     * sin_port indicates the port on which the server will wait for HTTP calls
     */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // address reachable only locally, from the same machine on which
                                                     // the server is started, in other cases you would need to put an
                                                     // IP address belonging to the machine running the server
    server.sin_port = htons(port);

    /*
     * the bind function is essential to bind the information we have put in "server"
     * to the reference to the socket we created; it needs:
     * 1. the reference to the socket
     * 2. the pointer to the memory area where the server info is stored
     * 3. size of the structure that contains the server info
     *
     * After this operation the socket stops being just a potential "port" and becomes
     * a real port bound to an IP address, so now it can work correctly
     */
    if (bind(socketReference, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        closesocket(socketReference);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d at address %s \n", ntohs(server.sin_port), inet_ntoa(server.sin_addr));

    /*
     * sets the socket to listen, where 3 is the size of the queue that can hold pending requests
     * waiting for those being processed to finish.
     * Changes the socket state, marking it as "passive" (does not initiate connections, but accepts them).
     * Builds a queue of connection requests: incoming connections are queued, and the server can then
     * accept them one at a time with accept. It does not yet accept any client: it only puts the server "ready to
     * listen".
     */
    listen(socketReference, 3);

    // variable of the same type as ‘server’, contains information such as the IP address and port of the client making
    // the request
    struct sockaddr_in client;
    // stores the size of the client variable in bytes
    int client_len = sizeof(client);
    int requestsCount = 0;
    while (1) {
        /*
         the "accept" function is used to accept the first request present in the listening queue
         It creates a new socket that will be used for communication with the client; It returns the number used by
         Windows to identify this communication channel at the hardware level. The accepted parameters are:
         1. socketReference = Windows reference to the server socket from which to extract the request
            (in other words, the queue from which to extract the request)
         2. the pointer to the memory area dedicated to the client, which will be filled with information of type
         sockaddr_in
         3. the size of the dedicated memory area, which will possibly be manipulated to be reallocated in
         case of insufficient space

         In case of error it returns the constant INVALID_SOCKET
         */
        SOCKET client_socket = accept(socketReference, (struct sockaddr *) &client, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Error accepting connection.\n");
            closesocket(socketReference);
            WSACleanup();
            return 1;
        }
        /*
         In case of successful connection, we print:
         1. The client's IP address, converted to string by inet_ntoa
         2. The port used by the client converted to number by ntohs
         */
        printf("Connection accepted by %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        requestsCount++;

        route_request(client_socket, client, client_len);

        if (requestsCount == 20)
            break;
    }

    printf("Press ENTER to close the server.\n");
    // function that waits for keyboard input, until it receives it "blocks" the execution, simulating
    // the server startup and waiting state
    getchar();


    closesocket(socketReference);
    // terminate the library and empty sockaddr_in
    WSACleanup();
    // disconnects the server from the database
    db_disconnect(db);
    return 0;
}
