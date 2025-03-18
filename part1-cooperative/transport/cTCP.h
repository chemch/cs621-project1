#ifndef C_TCP_H
#define C_TCP_H

#include <netinet/in.h>

// TCP Server
int create_tcp_server_socket(int port);
int accept_tcp_client(int server_sock, struct sockaddr_in *client_addr);

// TCP Client (Optional if needed later)
int create_tcp_client_socket(const char *server_ip, int port);

// Data Transfer
int recv_data(int sock, char *buffer, size_t size);
int send_data(int sock, const char *data, size_t size);

// Cleanup
void close_tcp_connection(int sock);

#endif