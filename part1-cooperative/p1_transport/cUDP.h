#ifndef C_UDP_H
#define C_UDP_H

#include "p1_app/compdetect_coop.h"

// ----------------------------------------
// Constants
// ----------------------------------------
#define DEF_UDP_SRC_PORT     9876
#define DEF_UDP_DST_PORT     8765
#define DEF_PACKET_SIZE      1024
#define MAX_PACKET_ID        65535

// ----------------------------------------
// Function Prototypes
// ----------------------------------------

/**
 * @brief Transmits a UDP packet train to the specified server.
 * 
 * @param client_ip    Source IP address.
 * @param server_ip    Destination IP address.
 * @param src_port     UDP source port.
 * @param dst_port     UDP destination port.
 * @param num_packets  Number of UDP packets to send.
 * @param packet_size  Size of each UDP packet (bytes).
 * @param entropy      Entropy level of payload (0 = low, 1 = high).
 * @param debug_mode   Enable verbose debug output.
 */
void transmit_udp_train(const char *client_ip, 
                        const char *server_ip, 
                        int src_port, 
                        int dst_port, 
                        int num_packets, 
                        int packet_size, 
                        int entropy, 
                        int debug_mode);

#endif  // C_UDP_H