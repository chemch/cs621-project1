#include "cFlow.h"

double run_train_flow(const char *client_ip, const char *server_ip,
                      int udp_src_port, int udp_dst_port,
                      int tcp_syn_x, int tcp_syn_y,
                      int udp_packet_count, int udp_payload_size,
                      int ttl, int debug_mode, int entropy) {

    struct timeval head_rst_time, tail_rst_time;

    printf("Sending HEAD SYN to %s:%d...\n", server_ip, tcp_syn_x);
    send_syn(client_ip, server_ip, tcp_syn_x);

    // Capture HEAD RST timestamp
    if (!capture_rst(tcp_syn_x, &head_rst_time)) {
        printf("Failed to detect due to missing HEAD RST.\n");
        return -1.0;
    }

    printf("Sending %s entropy UDP train...\n", entropy ? "HIGH" : "LOW");
    transmit_udp_train(client_ip, server_ip,
                       udp_src_port, udp_dst_port,
                       udp_packet_count, udp_payload_size,
                       entropy,
                       ttl, debug_mode);

    printf("Sending TAIL SYN to %s:%d...\n", server_ip, tcp_syn_y);
    send_syn(client_ip, server_ip, tcp_syn_y);

    // Capture TAIL RST timestamp
    if (!capture_rst(tcp_syn_y, &tail_rst_time)) {
        printf("Failed to detect due to missing TAIL RST.\n");
        return -1.0;
    }

    // Calculate delta in seconds
    double delta = (tail_rst_time.tv_sec - head_rst_time.tv_sec) +
                   (tail_rst_time.tv_usec - head_rst_time.tv_usec) / 1e6;

    printf("RST Arrival Delta: %.6f seconds\n", delta);

    return delta;
}
