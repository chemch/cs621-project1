#ifndef COMPDETECT_JSON_H
#define COMPDETECT_JSON_H

#include "compdetect.h"

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
/**
 *
 * @param configuration_file
 * @return parsed configuration struct from the input file
 */
Configuration read_configuration(const char *configuration_file) {

    // create a configuration object with the default values (in case some attributes are not provided in the file)
    Configuration configuration = {
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

    // create file stream to read from the input configuration
    FILE *raw_file = fopen(configuration_file, "r");

    // input parameter checking: make sure the file can be opened
    if (!raw_file) {
        perror("Error opening configuration file");
        exit(EXIT_FAILURE);
    }

    // determine file size
    fseek(raw_file, 0, SEEK_END);
    const long filesize = ftell(raw_file);
    rewind(raw_file);

    // read file contents into memory
    char *json_file = malloc(filesize + 1);
    fread(json_file, 1, filesize, raw_file);
    json_file[filesize] = '\0';

    // close the handle to the raw input file
    fclose(raw_file);

    // parse the json file into a cJSON object
    cJSON *json = cJSON_Parse(json_file);

    // validate that the json has been properly parsed to avoid errors later
    if (!json) {
        perror("Unable to parse the provided JSON. Check the Configuration file.");
        exit(EXIT_FAILURE);
    }

    // iterate through the json object and populate the configuration object with the values
    cJSON *json_item;
    if ((json_item = cJSON_GetObjectItem(json, "ServerIP")))
        strncpy(configuration.server_ip, json_item->valuestring, sizeof(configuration.server_ip));

    if ((json_item = cJSON_GetObjectItem(json, "UDPSourcePort")))
        configuration.udp_src_port = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "UDPDestinationPort")))
        configuration.udp_dst_port = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TCPSYNX")))
        configuration.tcp_syn_x = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TCPSYNY")))
        configuration.tcp_syn_y = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TCPPreProbePort")))
        configuration.tcp_pre_probe = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TCPPostProbePort")))
        configuration.tcp_post_probe = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "UDPPayloadSize")))
        configuration.udp_payload_size = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "InterMeasureTime")))
        configuration.inter_measure_time = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "UDPPacketCount")))
        configuration.udp_packet_count = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TTL")))
        configuration.ttl = json_item->valueint;

    // free the memory allocated for the json file
    cJSON_Delete(json);
    free(json_file);

    return configuration;
}

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
/**
 *
 * @param config
 */
void print_configuration(const Configuration *config) {
    printf("Configuration Settings:\n");
    printf("======================\n\n");
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
    printf("UDP TTL: %d\n", config->ttl);
    printf("\n");
}

#endif //COMPDETECT_JSON_H
