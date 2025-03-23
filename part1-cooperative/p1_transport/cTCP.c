#include "p1_transport/cTCP.h"


/** 
* @param port the port to bind the server socket to
* @return the server socket file descriptor
* @brief This function creates a tcp server socket and binds it to the specified port.
*/
int build_tcp_server_socket(int port) {
    int sock;
    struct sockaddr_in server_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fatal_error("FAILED TO CREATE TCP SOCKET.");
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // bind the socket to the server address
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        close(sock);
        fatal_error("FAILED TO BIND TCP SOCKET.");
    }

    // listen for incoming connections
    if (listen(sock, 5) == -1) {
        close(sock);
        fatal_error("FAILED TO LISTEN ON TCP SOCKET.");
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
        close(server_sock);
        fatal_error("FAILED TO ACCEPT CLIENT CONNECTION.");
    }
    return client_sock;
}

/** @param sock the socket file descriptor to send data on
 * @param data the data to send
 * @param size the size of the data to send
 * @return the number of bytes received
 * @brief This function receives data over a tcp connection.
*/
int receive_data(int sock, char *buffer, size_t size) {
    int bytes = recv(sock, buffer, size, 0);

    // warn the operator if receive fails or is empty
    if (bytes < 0) {
        warn("TCP RECEIVE FAILED.");
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
int transmit_data(int sock, const char *data, size_t size) {
    int bytes = send(sock, data, size, 0);

    // warn the operator if send fails
    if (bytes < 0) {
        warn("TCP SEND FAILED.");
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
