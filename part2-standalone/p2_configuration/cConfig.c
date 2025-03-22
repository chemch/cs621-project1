#include "cConfig.h"

/**
 * @file cConfig.c
 * @brief Parses JSON configuration for compression detection tool.
 *
 * This module reads a JSON file containing configuration settings and populates
 * a Configuration struct with user-provided values or sensible defaults.
 */

/**
 * @brief Reads and parses the configuration file into a Configuration struct.
 *
 * @param configuration_file Path to the JSON configuration file.
 * @return Parsed Configuration struct with default values where applicable.
 */
Configuration fetch_configuration(const char *configuration_file) {
    Configuration config = {
        .udp_src_port = DEF_UDP_SRC_PORT,
        .udp_dst_port = DEF_UDP_DST_PORT,
        .tcp_syn_x = DEF_TCP_SYN_X,
        .tcp_syn_y = DEF_TCP_SYN_Y,
        .udp_payload_size = DEF_UDP_PAYLOAD_SIZE,
        .inter_measure_time = DEF_INTER_MEASURE_TIME,
        .udp_packet_count = DEF_UDP_PACKET_COUNT,
        .ttl = DEF_UDP_TTL,
        .debug_mode = 0
    };

    // Open configuration file
    FILE *file = fopen(configuration_file, "r");
    if (!file) {
        fprintf(stderr, "ERROR OPENING CONFIGURATION FILE: %s\n", configuration_file);
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    // Allocate buffer and read file contents
    char *json_data = malloc(filesize + 1);
    if (!json_data) {
        fclose(file);
        fatal_error("Memory allocation failed");
    }

    size_t read_bytes = fread(json_data, 1, filesize, file);
    if (read_bytes != (size_t)filesize) {
        fclose(file);
        free(json_data);
        fatal_error("ERROR READING CONFIGURATION FILE CONTENTS\n");
    }
    json_data[filesize] = '\0';
    fclose(file);

    // Parse JSON
    cJSON *json = cJSON_Parse(json_data);
    free(json_data);

    if (!json) {
        fatal_error("ERROR PARSING JSON CONFIGURATION FILE");
    }

    // JSON value parsing macros
    #define CUSTOM_READ_STR_JSON_EL(key, target) \
        do { \
            cJSON *item = cJSON_GetObjectItemCaseSensitive(json, key); \
            if (cJSON_IsString(item) && item->valuestring) { \
                strncpy(target, item->valuestring, sizeof(target) - 1); \
                target[sizeof(target) - 1] = '\0'; \
            } \
        } while (0)

    #define CUSTOM_READ_INT_JSON_EL(key, target) \
        do { \
            cJSON *item = cJSON_GetObjectItemCaseSensitive(json, key); \
            if (cJSON_IsNumber(item)) { \
                target = item->valueint; \
            } \
        } while (0)

    // Extract config values with fallback defaults
    CUSTOM_READ_STR_JSON_EL("ClientIP", config.client_ip);                // Client's IP address
    CUSTOM_READ_STR_JSON_EL("ServerIP", config.server_ip);                // Server's IP address
    CUSTOM_READ_INT_JSON_EL("UDPSourcePort", config.udp_src_port);           // UDP source port
    CUSTOM_READ_INT_JSON_EL("UDPDestinationPort", config.udp_dst_port);      // UDP destination port
    CUSTOM_READ_INT_JSON_EL("TCPSYNX", config.tcp_syn_x);                    // TCP SYN head port
    CUSTOM_READ_INT_JSON_EL("TCPSYNY", config.tcp_syn_y);                    // TCP SYN tail port
    CUSTOM_READ_INT_JSON_EL("UDPPayloadSize", config.udp_payload_size);      // UDP payload size
    CUSTOM_READ_INT_JSON_EL("InterMeasureTime", config.inter_measure_time);  // Inter-measurement time
    CUSTOM_READ_INT_JSON_EL("UDPPacketCount", config.udp_packet_count);      // Number of UDP packets
    CUSTOM_READ_INT_JSON_EL("TTL", config.ttl);                              // TTL for UDP packets
    CUSTOM_READ_INT_JSON_EL("DebugMode", config.debug_mode);                 // Debug mode flag

    // Cleanup
    cJSON_Delete(json);
    #undef READ_STRING_JSON
    #undef READ_INT_JSON

    return config;
}
