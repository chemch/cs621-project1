#include "compdetect_alone.h"

/**
 * @file compdetect.c
 * @brief Standalone Compression Detection Tool
 *
 * This program implements a network compression detection mechanism using 
 * raw sockets. It sends TCP SYN packets and UDP packet trains with varying entropy 
 * to a remote server and captures TCP RST responses to measure timing differences 
 * that can indicate compression on the network path.
 *
 * ### Execution Flow:
 * 1. Loads configuration from a provided JSON file.
 * 2. Sends a HEAD SYN packet and records RST response time.
 * 3. Sends a UDP packet train with **low entropy** payload.
 * 4. Sends a TAIL SYN packet and records RST response time.
 * 5. Calculates timing delta for low entropy train.
 * 6. Waits for a configured inter-measurement period.
 * 7. Repeats steps 2–5 for a **high entropy** packet train.
 * 8. Computes the delta difference between high and low entropy runs.
 * 9. Determines whether compression is detected based on the delta difference.
 *
 * @usage ./compdetect <CONFIG.JSON>
 * 
 * @param CONFIG.JSON Path to a JSON configuration file with connection and transmission parameters.
 *
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error or insufficient data.
 *
 * @note This implementation requires root privileges to use raw sockets.
 */
int main(const int argc, char *argv[]) {

    // Validate input parameters
    if (argc != 2) {
        fatal_error("INVALID ARGUMENT COUNT. EXPECTED 1 ARGUMENT.");
    }

    // Read configuration from file
    Configuration config = fetch_configuration(argv[1]);

    // Validate server IP
    if (config.server_ip[0] == '\0') {
        fatal_error("INVALID SERVER CONFIGURATION FILE PROVIDED. BAD SERVER IP.\n");
    }

    // Run LOW entropy train
    double low_delta = process_entropy_flow(
        config.client_ip, 
        config.server_ip,
        config.udp_src_port, 
        config.udp_dst_port,
        config.tcp_syn_x, 
        config.tcp_syn_y,
        config.udp_packet_count, 
        config.udp_payload_size,
        config.ttl, 
        config.debug_mode, 
        0,
        config.inter_measure_time);

    if (low_delta < 0) {
        fatal_error("LOW ENTROPY RUN FAILED.\n");
    }

    // Sleep for inter-measurement time
    printf("SLEEPING FOR %d SECONDS BEFORE HIGH ENTROPY TRAIN...\n", config.inter_measure_time);
    sleep(config.inter_measure_time);

    // Run HIGH entropy train
    double high_delta = process_entropy_flow(
        config.client_ip, 
        config.server_ip,
        config.udp_src_port, 
        config.udp_dst_port,
        config.tcp_syn_x, 
        config.tcp_syn_y,
        config.udp_packet_count, 
        config.udp_payload_size,
        config.ttl, 
        config.debug_mode, 
        1,
        config.inter_measure_time);

    if (high_delta < 0) {
        fatal_error("HIGH ENTROPY RUN FAILED.\n");
    }

    // Calculate and print the difference
    double delta_delta = high_delta - low_delta;
    printf("DELTA DIFFERENCE (HIGH - LOW): %.6f SECONDS\n", delta_delta);

    if (delta_delta > DEF_COMPRESION_THRES) {
        printf("COMPRESSION DETECTED!\n");
    } else {
        printf("NO COMPRESSION DETECTED.\n");
    }

    return EXIT_SUCCESS;
}
