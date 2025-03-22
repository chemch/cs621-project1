#include "compdetect_coop.h"

#define PROBING_PHASE_DELAY 3

/**
 *
 * @param config_file to parse
 * @return configuration struct resulting from parsing
 * @brief This function runs the pre-probing phase of the application. It reads the configuration file
 *      and sends the configuration to the server.
 */
Configuration run_preprobing_phase(const char *config_file) {
    fprintf(stderr, "\t***PRE-PROBE PHASE STARTED***\n");

    // read the provided configuration file
    fprintf(stderr, "CONFIG FILE: %s\n\n", config_file);
    Configuration configuration = fetch_configuration(config_file);

    // validate that server ip address and port are valid
    if (configuration.server_ip[0] == '\0' || configuration.tcp_pre_probe <= 0) {
        fatal_error("INVALID SERVER CONFIGURATION FILE PROVIDED. BAD SERVER IP OR PORT.");
    }

    // print configuration for validation if in debug mode
    if (configuration.debug_mode) {
        print_configuration(&configuration);
    }

    // send the configuration to the server
    forward_configuration_to_server(&configuration);

    fprintf(stderr, "\t***PRE-PROBE PHASE COMPLETED SUCCESSFULLY!***\n\n");

    return configuration;
}

/**
 *
 * @param config to use for generating udp packet trains
 * @brief This function runs the probing phase of the application. It sends two UDP packet trains to the server
 *      to determine if compression is detected. The first train has low entropy, and the second train has high
 *     entropy. The time delta between the two trains is calculated and returned.
 * @note The time delta is used to determine if compression is detected.
 */
void run_probing_phase(const Configuration *config) {
    fprintf(stderr, "\t***PROBE PHASE STARTED***\n");

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

    fprintf(stderr, "\t***PROBE PHASE COMPLETED SUCCESSFULLY!***\n\n");
}

/**
 * receive calculated result back from server whether compression was detected, then print
 * 
 * @param config to use for generating udp packet trains
 * @brief This function runs the post-probing phase of the application. It communicates with the client
 *      to send the result of the compression detection based on the time delta between the high and low
 *     entropy trains.
 * @note The result message is sent to the client via a TCP connection.
 */
void run_postprobing_phase(const Configuration *config) {
    fprintf(stderr, "\t***POST-PROBE PHASE STARTED***\n");
    printf("SLEEPING %d SECONDS BEFORE POST PROBE PHASE TRANSMISSION...\n", config->inter_measure_time * 2);

    // sleep for the specified phase transition time
    sleep(config->inter_measure_time + PROBING_PHASE_DELAY);

    // create a socket for post-probing
    int sock;
    struct sockaddr_in server_addr;
    char buffer[256];

    // create tcp socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fatal_error("FAILED TO CREATE POST-PROBE SOCKET.");
    }

    // set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->tcp_post_probe);
    inet_pton(AF_INET, config->server_ip, &server_addr.sin_addr);

    // connect to server socket
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        close(sock);
        fatal_error("FAILED TO CONNECT TO SERVER.");
    }

    // receive results from calculation of server
    int received_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received_bytes <= 0) {
        close(sock);
        fatal_error("FAILED TO GET RESULTS OF CALCULATION FROM SERVER.");
    }

    // null-terminate the received data and print answer to part 1 task
    buffer[received_bytes] = '\0'; 
    printf("%s\n", buffer);

    close(sock);

    fprintf(stderr, "\t***POST-PROBE PHASE COMPLETED SUCCESSFULLY!***\n\n");
}

/**
 * client side of part 1 (cooperative) of the project to determine if compression is detected
* @param argc the number of arguments
* @param argv the arguments
* @return the exit status of the program
* @brief This function is the entry point for the client side of the application. It runs the pre-probing,
*       probing, and post-probing phases of the application to determine if compression is detected.
*      The client sends configuration data and UDP packet trains to the server. It then receives the
*     result of the compression detection from the server.
* @note The client is run with the command line arguments: <CONFIG.JSON>
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
