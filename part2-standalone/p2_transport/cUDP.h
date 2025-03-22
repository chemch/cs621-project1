#ifndef C_UDP_H
#define C_UDP_H

#include <stdint.h>  // For fixed-width integer types
#include <stddef.h>  // For size_t
#include "p2_app/compdetect_alone.h"  // Assuming Configuration struct and other dependencies

// --- Constants ---
#define UDP_SRC_PORT       9876   ///< Default UDP source port
#define UDP_DST_PORT       8765   ///< Default UDP destination port
#define PACKET_SIZE        1024   ///< Maximum size of UDP packet
#define MAX_PACKET_ID      65535  ///< Maximum UDP packet ID (16-bit)

// --- Function Prototypes ---

/**
 * @brief Transmits a train of UDP packets with specified entropy and payload.
 *
 * @param client_ip     Source IP address.
 * @param server_ip     Destination IP address.
 * @param src_port      Source UDP port.
 * @param dst_port      Destination UDP port.
 * @param num_packets   Number of packets to send.
 * @param packet_size   Size of each packet in bytes.
 * @param entropy       Entropy level (0 for low, 1 for high).
 * @param ttl           Time-To-Live for the IP header.
 * @param debug_mode    Enable verbose output if non-zero.
 */
void transmit_udp_train(const char *client_ip, const char *server_ip,
                        int src_port, int dst_port,
                        int num_packets, int packet_size,
                        int entropy, int ttl, int debug_mode);

#endif  // C_UDP_H