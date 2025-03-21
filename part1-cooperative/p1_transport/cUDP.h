#ifndef C_UDP_H
#define C_UDP_H

#include "p1_app/compdetect_coop.h"

#define UDP_SRC_PORT 9876
#define UDP_DST_PORT 8765
#define PACKET_SIZE 1024  
#define MAX_PACKET_ID 65535  

// function prototypes
void transmit_udp_train(const char *client_ip, const char *server_ip, int src_port, int dst_port, int num_packets, int packet_size, int entropy, int debug_mode);

#endif
