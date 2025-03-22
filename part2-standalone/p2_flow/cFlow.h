#ifndef C_FLOW_H
#define C_FLOW_H

#include "p2_app/compdetect_alone.h"

/**
 * @brief Thread arguments for capturing a single RST packet timestamp.
 */
struct rst_thread_args {
    int port;                                ///< Port to monitor for RST response
    struct timeval *timestamp;              ///< Pointer to store RST arrival timestamp
    int reset_timeout;                      ///< Time to wait for RST response
    int socket_timeout;                      ///< Socket timeout value
    int debug_mode;                         ///< Debug mode flag
};

/**
 * @brief Arguments for capturing RSTs from both HEAD and TAIL SYNs.
 * 
 * This struct is intended for more advanced usage where both ports are monitored,
 * and a time difference is calculated.
 */
struct rst_capture_args {
    int fd;                               ///< Raw socket file descriptor
    const char *head_port;                ///< HEAD port (as string)
    const char *tail_port;                ///< TAIL port (as string)
    double difference;                    ///< Time delta in milliseconds (optional)
    struct timespec head_rst_time;       ///< Timestamp of HEAD RST
    struct timespec tail_rst_time;       ///< Timestamp of TAIL RST
};


/**
 * @brief Thread function for capturing a single RST packet on a given port.
 *
 * @param arg Pointer to rst_thread_args_t struct
 * @return NULL
 */
void *monitor_reset_thread(void *arg);

/**
 * @brief Executes the entropy detection flow for one entropy level.
 *
 * @param client_ip       Source IP address
 * @param server_ip       Destination IP address
 * @param udp_src_port    Source UDP port
 * @param udp_dst_port    Destination UDP port
 * @param tcp_syn_x       Port for HEAD SYN packet
 * @param tcp_syn_y       Port for TAIL SYN packet
 * @param udp_packet_count Number of UDP packets in the train
 * @param udp_payload_size Payload size for each UDP packet
 * @param ttl             Time-to-live for IP packets
 * @param debug_mode      Verbosity flag (1 for verbose)
 * @param entropy_level   0 = Low entropy, 1 = High entropy
 * @return Delta in seconds between HEAD and TAIL RST packets, or -1.0 on failure
 */
double process_entropy_flow(const char *client_ip, const char *server_ip,
                        int udp_src_port, int udp_dst_port,
                        int tcp_syn_x, int tcp_syn_y,
                        int udp_packet_count, int udp_payload_size,
                        int ttl, int debug_mode, int entropy_level, int def_timeout);

#endif  // C_FLOW_H