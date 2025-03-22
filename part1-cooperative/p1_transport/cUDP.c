#include "cUDP.h"

/**
 * @param client_ip the client ip address
 * @param server_ip the server ip address
 * @param src_port the source port
 * @param dst_port the destination port
 * @param num_packets the number of packets to send
 * @param packet_size the size of each packet
 * @param entropy the entropy level of the packet payload
 * @param debug_mode the debug mode flag
 * 
 * @brief This function transmits a UDP train of packets to the server.
 * @note The function creates a UDP socket and binds to the source port. It then sends a specified number of packets
 *       to the server with a specified payload size and entropy level.
 */
void transmit_udp_train(const char *client_ip, const char *server_ip, int src_port, int dst_port, int num_packets, int packet_size, int entropy, int debug_mode) {
    int sock;
    struct sockaddr_in src_addr, dest_addr;
    char buffer[DEF_PACK_SIZE];

    // create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        fatal_error("FAILED TO CREATE SOCKET.");
    }

    printf("SOCKET SET UP SUCCESSFULLY.\n");

    // set dont fragment bit to 1
    int dont_fragment_bit = IP_PMTUDISC_DO;
    if (setsockopt(sock, IPPROTO_IP, IP_MTU_DISCOVER, &dont_fragment_bit, sizeof(dont_fragment_bit)) == -1) {
        close(sock);
        fatal_error("FAILED TO SET DONT FRAGMENT BIT.");
    }

    if(debug_mode) {
        printf("DEBUG MODE - DONT FRAGMENT BIT SET TO 1.\n");
    }

    // bind to the specified source ip and source port
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(src_port);
    src_addr.sin_addr.s_addr = inet_addr(client_ip); 

    // bind the socket to the source address
    if (bind(sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) == -1) {
        close(sock);
        fatal_error("FAILED TO BIND SOCKET TO SOURCE PORT.");
    }

    if(debug_mode) {
        printf("DEBUG MODE - SOURCE IP: %s, SOURCE PORT: %d BOUND SUCCESSFULLY.\n", client_ip, src_port);
    }

    // set up destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dst_port);
    inet_pton(AF_INET, server_ip, &dest_addr.sin_addr);

    // print out verbose logs if debug mode is enabled
    if(debug_mode) {
        printf("DEBUG MODE - SENDING %d %s ENTROPY PACKETS TO %s:%d...\n", 
            num_packets, entropy ? "HIGH" : "LOW", server_ip, dst_port); 
    }
    
    // transmit udp train
    for (int i = 0; i < num_packets; i++) {
        memset(buffer, 0, DEF_PACK_SIZE);

        // set packet id in the first two bytes
        buffer[0] = (i >> 8) & 0xFF;
        buffer[1] = i & 0xFF; 

        // fill the payload with low (zeros) or high (random) entropy data using /dev/urandom
        if (entropy) {
            int urand_fd = open("/dev/urandom", O_RDONLY);

            // throw if /dev/urandom fails to open
            if (urand_fd < 0) {
                fatal_error("FAILED TO OPEN /dev/urandom.");
            }
        
            // read random data from /dev/urandom
            ssize_t bytes_read = read(urand_fd, buffer + 2, packet_size - 2);
            if (bytes_read < packet_size - 2) {
                close(urand_fd);
                fatal_error("FAILED TO READ SUFFICIENT ENTROPY FROM /dev/urandom.");
            }
        
            // close the file descriptor
            close(urand_fd);
        } else {
            memset(buffer + 2, 0, packet_size - 2);
        }

        // transmit the packet
        if (sendto(sock, buffer, packet_size, 0, 
                   (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
            warn("FAILED TO SEND UDP PACKET.");
        } else {
            // print out verbose logs if debug mode is enabled
            if (debug_mode) {
                printf("DEBUG MODE - PACKET ID %d SENT TO %s:%d (%d BYTES)\n",
                       i, server_ip, dst_port, packet_size);
            }
        }
    }

    printf("SENT %s ENTROPY PACKETS SUCCESSFULLY.\n", entropy ? "HIGH" : "LOW");

    close(sock);
}
