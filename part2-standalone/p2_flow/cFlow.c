#include "cFlow.h"

/**
 * @brief Thread function to capture an RST packet on a specified port.
 *
 * @param arg Pointer to rst_thread_args containing the port and timestamp storage.
 * @return NULL
 */
void *capture_rst_thread(void *arg) {
    struct rst_thread_args *args = (struct rst_thread_args *)arg;
    if (!capture_rst(args->port, args->timestamp)) {
        fprintf(stderr, "FAILED TO CAPTURE RST ON PORT %d.\n", args->port);
    }
    return NULL;
}

/**
 * @brief Executes a complete entropy flow test with either low or high entropy.
 *
 * @param client_ip Source IP address
 * @param server_ip Destination IP address
 * @param udp_src_port UDP source port
 * @param udp_dst_port UDP destination port
 * @param tcp_syn_x Port for HEAD SYN
 * @param tcp_syn_y Port for TAIL SYN
 * @param udp_packet_count Number of UDP packets in train
 * @param udp_payload_size Size of each UDP packet payload
 * @param ttl Time-To-Live value for IP packets
 * @param debug_mode Debug flag (0 = off, 1 = on)
 * @param entropy Entropy flag (0 = low entropy, 1 = high entropy)
 * @return Delta time in seconds between HEAD and TAIL RST packets, or -1.0 on failure.
 */
double process_entropy_flow(const char *client_ip, const char *server_ip,
                        int udp_src_port, int udp_dst_port,
                        int tcp_syn_x, int tcp_syn_y,
                        int udp_packet_count, int udp_payload_size,
                        int ttl, int debug_mode, int entropy) {

    // Initialize RST capture threads and timestamps
    struct timeval head_rst_time, tail_rst_time;
    pthread_t head_thread, tail_thread;

    // Thread arguments for HEAD and TAIL RST capture
    struct rst_thread_args head_args = {tcp_syn_x, &head_rst_time};
    struct rst_thread_args tail_args = {tcp_syn_y, &tail_rst_time};

    // Start HEAD RST capture thread
    if (pthread_create(&head_thread, NULL, capture_rst_thread, &head_args) != 0) {
        perror("FAILED TO CREATE HEAD RST THREAD");
        return -1.0;
    }

    // Send HEAD SYN packet
    printf("SENDING HEAD SYN TO %s:%d...\n", server_ip, tcp_syn_x);
    send_syn(client_ip, server_ip, tcp_syn_x);

    // Wait for HEAD RST to be captured
    if (pthread_join(head_thread, NULL) != 0) {
        perror("FAILED TO JOIN HEAD RST THREAD");
        return -1.0;
    }

    // Transmit UDP train
    printf("SENDING %s ENTROPY UDP TRAIN...\n", entropy ? "HIGH" : "LOW");
    transmit_udp_train(client_ip, server_ip,
                       udp_src_port, udp_dst_port,
                       udp_packet_count, udp_payload_size,
                       entropy, ttl, debug_mode);

    // Start TAIL RST capture thread
    if (pthread_create(&tail_thread, NULL, capture_rst_thread, &tail_args) != 0) {
        perror("FAILED TO CREATE TAIL RST THREAD");
        return -1.0;
    }

    // Send TAIL SYN packet
    printf("SENDING TAIL SYN TO %s:%d...\n", server_ip, tcp_syn_y);
    send_syn(client_ip, server_ip, tcp_syn_y);

    // Wait for TAIL RST to be captured
    if (pthread_join(tail_thread, NULL) != 0) {
        perror("FAILED TO JOIN TAIL RST THREAD");
        return -1.0;
    }

    // Calculate delta in seconds
    double delta = (tail_rst_time.tv_sec - head_rst_time.tv_sec) +
                   (tail_rst_time.tv_usec - head_rst_time.tv_usec) / 1e6;

    printf("RST ARRIVAL DELTA: %.6F SECONDS\n", delta);
    return delta;
}
