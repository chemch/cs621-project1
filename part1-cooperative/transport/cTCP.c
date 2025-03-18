#include "transport/cTCP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Create and bind TCP server socket
int create_tcp_server_socket(int port) {
    int sock;
    struct sockaddr_in server_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("TCP SOCKET CREATION FAILED.");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("TCP SOCKET BIND FAILED.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 5) == -1) {
        perror("TCP LISTEN FAILED.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    return sock;
}

int accept_tcp_client(int server_sock, struct sockaddr_in *client_addr) {
    socklen_t len = sizeof(*client_addr);
    int client_sock = accept(server_sock, (struct sockaddr *)client_addr, &len);
    if (client_sock == -1) {
        perror("TCP ACCEPT FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    return client_sock;
}

int recv_data(int sock, char *buffer, size_t size) {
    int bytes = recv(sock, buffer, size, 0);
    if (bytes < 0) {
        perror("TCP RECEIVE FAILED.");
    }
    return bytes;
}

int send_data(int sock, const char *data, size_t size) {
    int bytes = send(sock, data, size, 0);
    if (bytes < 0) {
        perror("TCP SEND FAILED.");
    }
    return bytes;
}

void close_tcp_connection(int sock) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
}