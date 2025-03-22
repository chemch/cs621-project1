#ifndef C_RAW_TCP_H
#define C_RAW_TCP_H

#include "p2_app/compdetect_alone.h"

struct ipheader {
    unsigned char iph_l:4, iph_v:4;           // iph_v: IP version (always 4 for IPv4), iph_l: Header length in 32-bit words
    unsigned char iph_tos;                    // Type of Service (TOS), usually 0
    unsigned short int iph_tl;                // Total length of the IP packet (header + data) in bytes
    unsigned short int iph_id;                // Identification field for packet reassembly
    unsigned short int iph_offset;            // Fragmentation offset and flags
    unsigned char iph_ttl;                    // Time To Live (TTL), decremented by each router
    unsigned char iph_proto;                  // Protocol (e.g., TCP = 6, UDP = 17)
    unsigned short int iph_sum;               // Header checksum
    unsigned int iph_src;                     // Source IP address (network byte order)
    unsigned int iph_dst;                     // Destination IP address (network byte order)
};

struct tcpheader {
    unsigned short int th_sport;             // Source port number
    unsigned short int th_dport;             // Destination port number
    unsigned int th_seq;                     // Sequence number
    unsigned int th_ack;                     // Acknowledgment number (0 in SYN packets)
    unsigned char th_x2:4, th_off:4;         // th_off: Data offset (header length), th_x2: Reserved (usually 0)
    unsigned char th_flags;                  // TCP flags (SYN, ACK, RST, etc.)
    unsigned short int th_win;               // Window size
    unsigned short int th_sum;               // TCP checksum (includes pseudo-header)
    unsigned short int th_urp;               // Urgent pointer (unused in normal traffic)
};

struct pseudo_header {
    unsigned int source_address;             // Source IP address (same as in IP header)
    unsigned int dest_address;               // Destination IP address (same as in IP header)
    unsigned char placeholder;               // Always 0
    unsigned char protocol;                  // Protocol number (TCP = 6)
    unsigned short int tcp_length;           // Length of TCP header + data
};

// --- Function Prototypes ---

/**
 * @brief Sends a raw TCP SYN packet.
 * @param src_ip Source IP address
 * @param dst_ip Destination IP address
 * @param dst_port Destination port number
 * @return Socket file descriptor, or -1 on error
 */
int tcp_syn_transmission(const char *src_ip, const char *dst_ip, int dst_port);

/**
 * @brief Computes checksum for raw packet data.
 * @param buf Pointer to data buffer
 * @param size Length of buffer
 * @return Calculated checksum
 */
unsigned short computeChecksum(const char *buf, unsigned int size);

/**
 * @brief Captures an RST packet on a given port and retrieves its timestamp.
 * @param port Port to monitor for RST response
 * @param rst_timestamp Pointer to timeval to store arrival time
 * @return 1 if RST captured, 0 on timeout/failure
 */
int record_reset_packet(int port, struct timeval *rst_timestamp, int reset_timeout, int socket_timeout, int debug_mode);

#endif // C_RAW_TCP_H