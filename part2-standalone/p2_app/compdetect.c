# include "compdetect_alone.h"

int main(const int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "CORRECT USAGE: %s <CONFIG.JSON>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Configuration config = read_configuration(argv[1]);

    // validate that server ip address and port are valid
    if (config.server_ip[0] == '\0') {
        fprintf(stderr, "INVALID SERVER CONFIGURATION FILE PROVIDED.\n");
        exit(EXIT_FAILURE);
    }

    struct timeval head_rst_time, tail_rst_time;
    
    // Send HEAD SYN packet to port x
    printf("Sending HEAD SYN to %s:%d...\n", config.server_ip, config.tcp_syn_x);
    // send_syn(config.client_ip, config.server_ip, config.udp_src_port, config.tcp_syn_x);

    // Send LOW entropy UDP train
    printf("Sending LOW entropy UDP train...\n");
    transmit_udp_train(config.client_ip, config.server_ip,
                        config.udp_src_port, config.udp_dst_port,
                        config.udp_packet_count, config.udp_payload_size,
                        0,  // 0 = low entropy
                        config.ttl, config.debug_mode);

    // Send TAIL SYN packet to port y
    printf("Sending TAIL SYN to %s:%d...\n", config.server_ip, config.tcp_syn_y);
    // send_syn(config.client_ip, config.server_ip, config.udp_src_port, config.tcp_syn_y);

    // Send HIGH entropy UDP train
    printf("Sending HIGH entropy UDP train...\n");
    transmit_udp_train(config.client_ip, config.server_ip,
                        config.udp_src_port, config.udp_dst_port,
                        config.udp_packet_count, config.udp_payload_size,
                        1,  // 1 = high entropy
                        config.ttl, config.debug_mode);

    printf("Transmission complete.\n");
    return EXIT_SUCCESS;
}