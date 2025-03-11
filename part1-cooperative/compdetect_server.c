#include "compdetect.h"
#include "compdetect_config.h"

/**
 *
 * @param port to connect to tcp client on
 */
Configuration run_preprobing_phase(int port) {

    // define socket parameters for the client and server
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // define buffer for receiving data from the client`
    char buffer[DEF_BUFFER_SIZE];

    // create new tcp socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket!");
        exit(EXIT_FAILURE);
    }

    // server socket settings
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // try to bind the server to the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed!");
        exit(EXIT_FAILURE);
    }

    // wait for incoming connection
    if (listen(server_sock, 5) == -1) {
        perror("Listening failed on server!");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    // accept incoming client connection
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("Connection acceptance failed!!");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Client connected using tcp source port %d\n", ntohs(client_addr.sin_port));

    // receive JSON data from client
    const int received_bytes = recv(client_sock, buffer, DEF_BUFFER_SIZE - 1, 0);
    if (received_bytes <= 0) {
        perror("Failed to receive data");
        exit(EXIT_FAILURE);
    }

    // terminate the string
    buffer[received_bytes] = '\0';

    printf("Received Configuration from Client...\n");

    // parse the JSON
    cJSON *json = cJSON_Parse(buffer);

    // validate that the json was parsed successfully
    if (!json) {
        fprintf(stderr, "Error parsing received JSON!!!\n");
        exit(EXIT_FAILURE);
    }

    // Convert JSON object to Configuration struct
    Configuration config;
    if (!json_to_configuration(json, &config)) {
        fprintf(stderr, "Unable to convert received JSON to a Configuration struct...\n");
        cJSON_Delete(json);
        close(client_sock);
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // print the configuration parsed from client
    printf("Configuration Settings Received:\n");
    printf("======================\n\n");
    print_configuration(&config);

    // free the memory allocated for the json object
    cJSON_Delete(json);

    // close the handles to the client and server sockets
    close(client_sock);
    close(server_sock);

    return config;
}

/**
 *
 */
void run_probing_phase(const Configuration *config) {
    printf("Running Probing Phase...\n");
    printf("Using UDP Source Port: %d\n", config->udp_src_port);
    printf("Using UDP Destination Port: %d\n", config->udp_dst_port);
    // Implement probing logic using configuration settings
}

/**
 *
 */
void run_postprobing_phase() {}


/*
 * PART 1 - Server Side: Compression Detection Client/Server Application
*/
int main(const int argc, char *argv[]) {

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

    // pre-probing phase will allow the server to receive the configuration from the client via a tcp connection
    Configuration configuration = run_preprobing_phase(port);

    // probing phase will allow the server to receive 2 udp packet trains, 1 w/ high entropy and 1 w/ low entropy
    run_probing_phase(&configuration);

    // post-probing phase will allow the server calculate compression detection and communicate that back to the client
    run_postprobing_phase();

    // exit will never hit
    return EXIT_SUCCESS;
}
