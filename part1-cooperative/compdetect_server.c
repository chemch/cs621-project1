#include "compdetect.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "cJSON.h"

#define BUFFER_SIZE 4096


/**
 *
 * @param port to connect to tcp client on
 */
void run_server(int port) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to the specified port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    // Accept connection
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("Connection acceptance failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected!\n");

    // Receive JSON data
    int received_bytes = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (received_bytes <= 0) {
        perror("Failed to receive data");
        exit(EXIT_FAILURE);
    }
    buffer[received_bytes] = '\0';  // Null terminate the received data

    printf("Received Configuration: %s\n", buffer);

    // Parse JSON
    cJSON *json = cJSON_Parse(buffer);
    if (!json) {
        fprintf(stderr, "Error parsing received JSON\n");
        exit(EXIT_FAILURE);
    }

    // Extract values (example)
    const cJSON *server_ip_item = cJSON_GetObjectItem(json, "ServerIP");
    if (cJSON_IsString(server_ip_item)) {
        printf("Server IP from client: %s\n", server_ip_item->valuestring);
    }

    cJSON_Delete(json);
    close(client_sock);
    close(server_sock);
}


/*
 * PART 1 - Server Side: Compression Detection Client/Server Application
*/
int main(int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "Correct Usage: %s <tcp port>\n", argv[0]);

        // exit with failure
        return EXIT_FAILURE;
    }

    // set the port from the provided command line input, perform validation it is a valid tcp port number
    const int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error due to the provided tcp port. Tcp ports must be between 1 and 65535.\n");
        return EXIT_FAILURE;
    }

    // otherwise, run the server
    run_server(port);

    // exit will never hit
    return EXIT_SUCCESS;
}
