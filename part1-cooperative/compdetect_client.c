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
    // pause phase transition time before starting packet trains
    sleep(config->inter_measure_time);
    

    printf("Running Probing Phase...\n");
    send_udp_packets(config->server_ip, config->udp_src_port, config->udp_dst_port,
                     config->udp_packet_count, config->udp_payload_size, 0);

    printf("Waiting for %d seconds before high entropy transmission...\n", config->inter_measure_time);
    sleep(config->inter_measure_time);

    send_udp_packets(config->server_ip, config->udp_src_port, config->udp_dst_port,
                     config->udp_packet_count, config->udp_payload_size, 1);

    print_configuration(config);
}


/**
 * receive calculated result back from server whether compression was detected, then print
 */
void run_postprobing_phase(const Configuration *config) {
    fprintf(stderr, "Post-Probing Phase\n");
    fprintf(stderr, "=================\n\n");
    print_configuration(config);
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
    // run_postprobing_phase(&configuration);

    return EXIT_SUCCESS;
}
