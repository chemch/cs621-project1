#ifndef COMPDETECT_JSON_H
#define COMPDETECT_JSON_H

#include "compdetect.h"

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
Config read_config(const char *filename) {
    Config config = {
        .udp_src_port = DEF_UDP_SRC_PORT,
        .udp_dst_port = DEF_UDP_DST_PORT,
        .tcp_syn_x = DEF_TCP_SYN_X,
        .tcp_syn_y = DEF_TCP_SYN_Y,
        .tcp_pre_probe = DEF_TCP_PRE_PROBE,
        .tcp_post_probe = DEF_TCP_POST_PROBE,
        .udp_payload_size = DEF_UDP_PAYLOAD_SIZE,
        .inter_measure_time = DEF_INTER_MEASURE_TIME,
        .udp_packet_count = DEF_UDP_PACKET_COUNT,
        .ttl = DEF_UDP_TTL
    };

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening configuration file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    char *json_data = (char *)malloc(filesize + 1);
    fread(json_data, 1, filesize, file);
    json_data[filesize] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(json_data);
    if (!json) {
        perror("Error parsing JSON configuration");
        exit(EXIT_FAILURE);
    }

    cJSON *item;
    if ((item = cJSON_GetObjectItem(json, "ServerIP")))
        strncpy(config.server_ip, item->valuestring, sizeof(config.server_ip));
    if ((item = cJSON_GetObjectItem(json, "UDPSourcePort")))
        config.udp_src_port = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "UDPDestinationPort")))
        config.udp_dst_port = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "TCPSYNX")))
        config.tcp_syn_x = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "TCPSYNY")))
        config.tcp_syn_y = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "TCPPreProbePort")))
        config.tcp_pre_probe = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "TCPPostProbePort")))
        config.tcp_post_probe = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "UDPPayloadSize")))
        config.udp_payload_size = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "InterMeasureTime")))
        config.inter_measure_time = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "UDPPacketCount")))
        config.udp_packet_count = item->valueint;
    if ((item = cJSON_GetObjectItem(json, "TTL")))
        config.ttl = item->valueint;

    cJSON_Delete(json);
    free(json_data);

    return config;
}

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
void print_config(const Config *config) {
    printf("Configuration Values:\n");
    printf("Server IP: %s\n", config->server_ip);
    printf("UDP Source Port: %d\n", config->udp_src_port);
    printf("UDP Destination Port: %d\n", config->udp_dst_port);
    printf("TCP SYN X Port: %d\n", config->tcp_syn_x);
    printf("TCP SYN Y Port: %d\n", config->tcp_syn_y);
    printf("TCP Pre-Probe Port: %d\n", config->tcp_pre_probe);
    printf("TCP Post-Probe Port: %d\n", config->tcp_post_probe);
    printf("UDP Payload Size: %dB\n", config->udp_payload_size);
    printf("Inter-Measurement Time: %d seconds\n", config->inter_measure_time);
    printf("Number of UDP Packets: %d\n", config->udp_packet_count);
    printf("TTL: %d\n", config->ttl);
}

#endif //COMPDETECT_JSON_H
