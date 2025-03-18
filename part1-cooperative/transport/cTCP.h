#ifndef C_TCP_H
#define C_TCP_H

#include <netinet/in.h>

// tcp server setup
int build_tcp_server_socket(int port);
int accept_tcp_client(int server_sock, struct sockaddr_in *client_addr);

// data transfer functions
int recv_data(int sock, char *buffer, size_t size);
int send_data(int sock, const char *data, size_t size);

// cleanup and close sockets
void close_tcp_connection(int sock);

#endif