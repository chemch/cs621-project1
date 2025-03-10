#ifndef COMPDETECT_H
#define COMPDETECT_H

// required external dependencies
// ReSharper disable CppUnusedIncludeDirective
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

// required local dependency (downloaded and added to root path)
#include "cJSON.h"

// application defaults (per project 1 instructions)
#define DEF_UDP_SRC_PORT 9876
#define DEF_UDP_DST_PORT 8765
#define DEF_TCP_SYN_X 9999
#define DEF_TCP_SYN_Y 8888
#define DEF_TCP_PRE_PROBE 7777
#define DEF_TCP_POST_PROBE 6666
#define DEF_UDP_PAYLOAD_SIZE 1000
#define DEF_INTER_MEASURE_TIME 15
#define DEF_UDP_PACKET_COUNT 6000
#define DEF_UDP_TTL 255

// buffer size for communication between client and server
#define DEF_BUFFER_SIZE 4096

// custom struct definition for configuration
typedef struct {
    char server_ip[64];
    int udp_src_port;
    int udp_dst_port;
    int tcp_syn_x;
    int tcp_syn_y;
    int tcp_pre_probe;
    int tcp_post_probe;
    int udp_payload_size;
    int inter_measure_time;
    int udp_packet_count;
    int ttl;
} Configuration;

#endif //COMPDETECT_H
