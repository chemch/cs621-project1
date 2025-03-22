#ifndef C_CONFIG_H
#define C_CONFIG_H

#include "p2_app/compdetect_alone.h"  // Ensure this is needed; minimize dependencies

// -----------------------------------------------------------------------------
// Configuration Structure Definition
// -----------------------------------------------------------------------------

/**
 * @brief Configuration settings for compression detection.
 *
 * This struct holds user-defined or default configuration values parsed from
 * a JSON file, which are used throughout the application for network probing.
 */
typedef struct {
    char client_ip[64];          ///< IP address of the client
    char server_ip[64];          ///< IP address of the server
    int udp_src_port;            ///< Source port for UDP packets
    int udp_dst_port;            ///< Destination port for UDP packets
    int tcp_syn_x;               ///< Destination port for HEAD SYN packet
    int tcp_syn_y;               ///< Destination port for TAIL SYN packet
    int udp_payload_size;        ///< Size of the UDP payload in bytes
    int inter_measure_time;      ///< Time delay between measurements (seconds)
    int udp_packet_count;        ///< Number of UDP packets in the train
    int ttl;                     ///< Time-To-Live value for UDP packets
    int debug_mode;              ///< Debug mode flag (0 = off, 1 = on)
} Configuration;

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------

/**
 * @brief Reads configuration from a JSON file.
 *
 * @param configuration_file Path to JSON configuration file.
 * @return Parsed Configuration struct.
 */
Configuration fetch_configuration(const char *configuration_file);

#endif  // C_CONFIG_H