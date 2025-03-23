#ifndef C_CONFIG_H
#define C_CONFIG_H

#include "p1_app/compdetect_coop.h"

// ----------------------------------------
// Configuration Structure
// ----------------------------------------

/**
 * @brief Holds all configuration parameters parsed from a JSON file.
 */
typedef struct {
    char client_ip[64];           ///< IP address of the client.
    char server_ip[64];           ///< IP address of the server.
    int udp_src_port;             ///< UDP source port.
    int udp_dst_port;             ///< UDP destination port.
    int tcp_syn_x;                ///< TCP SYN X port.
    int tcp_syn_y;                ///< TCP SYN Y port.
    int tcp_pre_probe;            ///< TCP pre-probe port.
    int tcp_post_probe;           ///< TCP post-probe port.
    int udp_payload_size;         ///< Size of each UDP payload in bytes.
    int inter_measure_time;       ///< Time (seconds) between measurements.
    int udp_packet_count;         ///< Number of UDP packets in a train.
    int ttl;                      ///< Time-to-live for UDP packets.
    int debug_mode;               ///< Debug mode flag (0 = off, 1 = on).
} Configuration;

// ----------------------------------------
// Configuration Function Prototypes
// ----------------------------------------

/**
 * @brief Parses the JSON configuration file and returns a Configuration struct.
 *
 * @param configuration_file Path to the JSON file.
 * @return Parsed Configuration struct.
 */
Configuration fetch_configuration(const char *configuration_file);

/**
 * @brief Prints the contents of a Configuration struct for debugging.
 *
 * @param config Pointer to the Configuration to print.
 */
void print_configuration(const Configuration *config);

/**
 * @brief Sends the configuration to the server as a JSON string.
 *
 * @param configuration Pointer to the Configuration to forward.
 */
void forward_configuration_to_server(Configuration *configuration);

/**
 * @brief Converts a cJSON object into a Configuration struct.
 *
 * @param json Parsed JSON object.
 * @param config Pointer to the Configuration struct to populate.
 * @return 1 on success, 0 on failure.
 */
int json_to_configuration(cJSON *json, Configuration *config);

/**
 * @brief Converts a Configuration struct into a JSON-formatted string.
 *
 * @param config Pointer to the Configuration to convert.
 * @return JSON string representation (caller must free), or NULL on failure.
 */
char *convert_configuration_to_json(Configuration *config);

#endif  // C_CONFIG_H
