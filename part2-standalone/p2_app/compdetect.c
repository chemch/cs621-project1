#include "compdetect_alone.h"

int main(const int argc, char *argv[]) {

    // Validate input parameters
    if (argc != 2) {
        fprintf(stderr, "CORRECT USAGE: %s <CONFIG.JSON>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Read configuration from file
    Configuration config = read_configuration(argv[1]);

    // Validate server IP
    if (config.server_ip[0] == '\0') {
        fprintf(stderr, "INVALID SERVER CONFIGURATION FILE PROVIDED.\n");
        return EXIT_FAILURE;
    }

    // Run LOW entropy train
    double low_delta = run_train_flow(config.client_ip, config.server_ip,
        config.udp_src_port, config.udp_dst_port,
        config.tcp_syn_x, config.tcp_syn_y,
        config.udp_packet_count, config.udp_payload_size,
        config.ttl, config.debug_mode, 0);  // 0 = LOW entropy

    if (low_delta < 0) {
        printf("Low entropy run failed.\n");
        return EXIT_FAILURE;
    }

    // Run HIGH entropy train
    double high_delta = run_train_flow(config.client_ip, config.server_ip,
        config.udp_src_port, config.udp_dst_port,
        config.tcp_syn_x, config.tcp_syn_y,
        config.udp_packet_count, config.udp_payload_size,
        config.ttl, config.debug_mode, 1);  // 1 = HIGH entropy

    if (high_delta < 0) {
        printf("High entropy run failed.\n");
        return EXIT_FAILURE;
    }

    // Calculate and print the difference
    double delta_difference = high_delta - low_delta;
    printf("Delta Difference (High - Low): %.6f seconds\n", delta_difference);

    if (delta_difference > 0.1) {
        printf("Compression Detected!\n");
    } else {
        printf("No Compression Detected.\n");
    }

    printf("Transmission complete.\n");
    return EXIT_SUCCESS;
}
