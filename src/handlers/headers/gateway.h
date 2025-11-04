#ifndef HANDLER_H
#define HANDLER_H

#include <winsock2.h>

int route_request(SOCKET listen_sock,struct sockaddr_in client_addr,int addrlen);

#endif