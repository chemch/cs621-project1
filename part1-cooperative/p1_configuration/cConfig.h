#ifndef C_CONFIG_H
#define C_CONFIG_H

#include "p1_app/compdetect_coop.h"

// custom struct definition for configuration
typedef struct {
    char client_ip[64];
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
    int debug_mode;
} Configuration;

Configuration read_configuration(const char *configuration_file);
void print_configuration(const Configuration *config);
void forward_configuration_to_server(Configuration *configuration);
int json_to_configuration(cJSON *json, Configuration *config);
char *convert_configuration_to_json(Configuration *config);

#endif
