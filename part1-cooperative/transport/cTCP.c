#include "transport/cTCP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


/** 
* @param port the port to bind the server socket to
* @return the server socket file descriptor
* @brief This function creates a tcp server socket and binds it to the specified port.
*/
int build_tcp_server_socket(int port) {
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

/** 
 * @param server_sock the server socket file descriptor
 * @param client_addr the client address struct to store the client address
 * @return the client socket file descriptor
 * @brief This function accepts a tcp client connection and returns the client socket file descriptor.
*/
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

/** @param sock the socket file descriptor to send data on
 * @param data the data to send
 * @param size the size of the data to send
 * @return the number of bytes received
 * @brief This function receives data over a tcp connection.
*/
int recv_data(int sock, char *buffer, size_t size) {
    int bytes = recv(sock, buffer, size, 0);
    if (bytes < 0) {
        perror("TCP RECEIVE FAILED.");
    }
    return bytes;
}

/** 
* @param sock the socket file descriptor to send data on
* @param data the data to send
* @param size the size of the data to send
* @return the number of bytes sent
* @brief This function sends data over a tcp connection.
*/
int send_data(int sock, const char *data, size_t size) {
    int bytes = send(sock, data, size, 0);
    if (bytes < 0) {
        perror("TCP SEND FAILED.");
    }
    return bytes;
}

/** 
 * @param sock the socket file descriptor to close
 * @brief This function closes a tcp connection by shutting down the socket and closing it.
*/
void close_tcp_connection(int sock) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
}
