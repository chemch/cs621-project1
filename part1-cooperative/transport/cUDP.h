#ifndef COMPDETECT_UDP_H
#define COMPDETECT_UDP_H

#include "app/compdetect.h"

#define UDP_SRC_PORT 9876
#define UDP_DST_PORT 8765
#define PACKET_SIZE 1024  // UDP Packet size including payload
#define MAX_PACKET_ID 65535  // 16-bit Packet ID limit

// Structure for pseudo header for checksum calculation
struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

// Function Prototypes
void send_udp_packets(const char *client_ip, const char *server_ip, int src_port, int dst_port, int num_packets, int packet_size, int entropy);



#endif //COMPDETECT_UDP_H
