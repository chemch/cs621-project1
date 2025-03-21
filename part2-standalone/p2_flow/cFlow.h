#ifndef C_FLOW_H
#define C_FLOW_H

#include "p2_app/compdetect_alone.h"


// Thread argument struct for RST capture
struct rst_thread_args {
    int port;
    struct timeval *timestamp;
};

// Declare the thread function
void *capture_rst_thread(void *arg);

struct arg_struct {
    int fd;
    char *head_port;
    char *tail_port;
    double difference;               // Time delta in milliseconds (optional)
    struct timespec head_rst_time;  // Timestamp of HEAD RST
    struct timespec tail_rst_time;  // Timestamp of TAIL RST
};

double run_train_flow(const char *client_ip, const char *server_ip,
    int udp_src_port, int udp_dst_port,
    int tcp_syn_x, int tcp_syn_y,
    int udp_packet_count, int udp_payload_size,
    int ttl, int debug_mode, int entropy_level);

#endif