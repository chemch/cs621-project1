#include "compdetect.h"

#define PROBING_PHASE_DELAY 2

/**
 *
 * @param config_file to parse
 * @return configuration struct resulting from parsing
 */
Configuration run_preprobing_phase(const char *config_file) {

    // read the provided configuration file
    fprintf(stderr, "CONFIG FILE: %s\n\n", config_file);
    Configuration configuration = read_configuration(config_file);

    // validate that server ip address and port are valid
    if (configuration.server_ip[0] == '\0' || configuration.tcp_pre_probe <= 0) {
        fprintf(stderr, "INVALID SERVER CONFIGURATION FILE PROVIDED.\n");
        exit(EXIT_FAILURE);
    }

    // print configuration for validation
    print_configuration(&configuration);

    // send the configuration to the server
    forward_configuration_to_server(&configuration);

    return configuration;
}

/**
 *
 * @param config to use for generating udp packet trains
 */
void run_probing_phase(const Configuration *config) {

    // sleep for a few seconds before starting the probing phase
    printf("SLEEPING %d SECONDS PRIOR TO LOW ETROPY TRAIN...\n", PROBING_PHASE_DELAY);
    sleep(PROBING_PHASE_DELAY);

    transmit_udp_train(config->client_ip, 
        config->server_ip, 
        config->udp_src_port, 
        config->udp_dst_port,               
        config->udp_packet_count, 
        config->udp_payload_size, 
        0, 
        config->debug_mode);

    // sleep for the specified inter-measure time before sending the high entropy train
    printf("SLEEPING %d SECONDS PRIOR TO HIGH ENTROPY TRAIN...\n", config->inter_measure_time);
    sleep(config->inter_measure_time);

    transmit_udp_train(
        config->client_ip, 
        config->server_ip, 
        config->udp_src_port, 
        config->udp_dst_port,         
        config->udp_packet_count, 
        config->udp_payload_size, 
        1, 
        config->debug_mode);

    printf("PROBING PHASE COMPLETED SUCCESFULLY.\n");
}

/**
 * receive calculated result back from server whether compression was detected, then print
 * 
 * @param config to use for generating udp packet trains
 */
void run_postprobing_phase(const Configuration *config) {
    printf("SLEEPING %f SECONDS BEFORE POST PROBE PHASE...\n", config->inter_measure_time * 2);

    // sleep for the specified phase transition time
    sleep(config->inter_measure_time * 2);

    // create a socket for post-probing
    int sock;
    struct sockaddr_in server_addr;
    char buffer[256];

    // create tcp socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("FAILED TO CEATE POST-PROBE SOCKET.");
        exit(EXIT_FAILURE);
    }

    // set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->tcp_post_probe);
    inet_pton(AF_INET, config->server_ip, &server_addr.sin_addr);

    // connect to server socket
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("FAILED TO CONNECT TO SERVER.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // receive results from calculation of server
    int received_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received_bytes <= 0) {
        perror("FAILED TO GET RESULTS OF CALCULATION FROM SERVER.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // null-terminate the received data and print answer to part 1 task
    buffer[received_bytes] = '\0'; 
    printf("%s\n", buffer);

    close(sock);
}

/*
 * client side of part 1 (cooperative) of the project to determine if compression is detected
*/
int main(const int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "CORRECT USAGE: %s <CONFIG.JSON>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // pre-probing phase: read configuration and send to server
    const Configuration configuration = run_preprobing_phase(argv[1]);

    // probing phase: send 2 udp packet trains to server, one with high entropy and another with low
    run_probing_phase(&configuration);

    // post-probing phase: receive results back from server on compression calc results and print
    run_postprobing_phase(&configuration);

    return EXIT_SUCCESS;
}
