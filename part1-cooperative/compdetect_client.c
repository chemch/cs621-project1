#include "compdetect.h"

/**
 *
 * @param config_file to parse
 * @return configuration struct resulting from parsing
 */
Configuration run_preprobing_phase(const char *config_file) {
    // read the provided configuration file
    fprintf(stderr, "Configuration File: %s\n\n", config_file);
    Configuration configuration = read_configuration(config_file);

    // Validate that server IP and port are valid
    if (configuration.server_ip[0] == '\0' || configuration.tcp_pre_probe <= 0) {
        fprintf(stderr, "Error: Invalid Server Configuration Read from Config\n");
        exit(EXIT_FAILURE);
    }

    // Print configuration for validation
    print_configuration(&configuration);

    // Send the configuration to the server
    forward_configuration_to_server(&configuration);

    return configuration;
}

/**
 *
 * @param config to use for generating udp packet trains
 */
void run_probing_phase(const Configuration *config) {
    printf("Waiting for %d seconds before probing...\n", 2);

    // sleep for the specified phase transition time
    sleep(2);

    printf("Running Probing Phase...\n");
    send_udp_packets(config->client_ip, config->server_ip, config->udp_src_port, config->udp_dst_port,
                     config->udp_packet_count, config->udp_payload_size, 0);

    printf("Waiting for %d seconds before high entropy transmission...\n", config->inter_measure_time);
    sleep(config->inter_measure_time);

    send_udp_packets(config->client_ip, config->server_ip, config->udp_src_port, config->udp_dst_port,
                     config->udp_packet_count, config->udp_payload_size, 1);
}


/**
 * receive calculated result back from server whether compression was detected, then print
 */
void run_postprobing_phase(const Configuration *config) {
    printf("Waiting for %f seconds before postprobing...\n", config->inter_measure_time * 1.2);

    // sleep for the specified phase transition time
    sleep(config->inter_measure_time);

    int sock;
    struct sockaddr_in server_addr;
    char buffer[256];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Post-Probing TCP socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->tcp_post_probe);
    inet_pton(AF_INET, config->server_ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Post-Probing TCP connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Receive result
    int received_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received_bytes <= 0) {
        perror("Failed to receive result from server");
        close(sock);
        exit(EXIT_FAILURE);
    }

    buffer[received_bytes] = '\0';  // Null-terminate the received string
    printf("Server Result: %s\n", buffer);

    close(sock);
}

/*
 * PART 1 - Client Side: Compression Detection Client/Server Application
*/
int main(const int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "Correct Usage: %s <configuration.json>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // pre-probing phase: Read configuration and send to server
    const Configuration configuration = run_preprobing_phase(argv[1]);

    // probing phase: Send 2 UDP packet trains to server, one with high entropy and another with low
    run_probing_phase(&configuration);

    // post-probing phase: receive results back from server on compression calc results and print
    run_postprobing_phase(&configuration);

    return EXIT_SUCCESS;
}
