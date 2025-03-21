#include "cFlow.h"

void *capture_rst_thread(void *arg) {
    struct rst_thread_args *args = (struct rst_thread_args *)arg;
    capture_rst(args->port, args->timestamp);
    return NULL;
}

double run_train_flow(const char *client_ip, const char *server_ip,
    int udp_src_port, int udp_dst_port,
    int tcp_syn_x, int tcp_syn_y,
    int udp_packet_count, int udp_payload_size,
    int ttl, int debug_mode, int entropy) {

    struct timeval head_rst_time, tail_rst_time;

    pthread_t head_thread, tail_thread;
    struct rst_thread_args head_args = {tcp_syn_x, &head_rst_time};
    struct rst_thread_args tail_args = {tcp_syn_y, &tail_rst_time};

    // Start HEAD RST capture thread
    if (pthread_create(&head_thread, NULL, capture_rst_thread, &head_args) != 0) {
    perror("Failed to create HEAD RST thread");
    return -1.0;
    }

    printf("Sending HEAD SYN to %s:%d...\n", server_ip, tcp_syn_x);
    send_syn(client_ip, server_ip, tcp_syn_x);

    // Wait for HEAD RST to be captured
    pthread_join(head_thread, NULL);

    printf("Sending %s entropy UDP train...\n", entropy ? "HIGH" : "LOW");
    transmit_udp_train(client_ip, server_ip,
        udp_src_port, udp_dst_port,
        udp_packet_count, udp_payload_size,
        entropy, ttl, debug_mode);

    // Start TAIL RST capture thread
    if (pthread_create(&tail_thread, NULL, capture_rst_thread, &tail_args) != 0) {
    perror("Failed to create TAIL RST thread");
    return -1.0;
    }

    printf("Sending TAIL SYN to %s:%d...\n", server_ip, tcp_syn_y);
    send_syn(client_ip, server_ip, tcp_syn_y);

    // Wait for TAIL RST to be captured
    pthread_join(tail_thread, NULL);

    // Calculate delta in seconds
    double delta = (tail_rst_time.tv_sec - head_rst_time.tv_sec) +
    (tail_rst_time.tv_usec - head_rst_time.tv_usec) / 1e6;

    printf("RST Arrival Delta: %.6f seconds\n", delta);
    return delta;
}
