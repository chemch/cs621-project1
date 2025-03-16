#include "cUDP.h"

void send_udp_packets(const char *client_ip, const char *server_ip, int src_port, int dst_port, int num_packets, int packet_size, int entropy) {
    int sock;
    struct sockaddr_in src_addr, dest_addr;
    char buffer[PACKET_SIZE];

    // Create a UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("[ERROR] UDP socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("[DEBUG] UDP socket created successfully.\n");

    // Set Don't Fragment (DF) bit
    int df = IP_PMTUDISC_DO;
    if (setsockopt(sock, IPPROTO_IP, IP_MTU_DISCOVER, &df, sizeof(df)) == -1) {
        perror("[ERROR] setsockopt DF bit failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("[DEBUG] DF bit set successfully.\n");

    // Bind the socket to the specified source IP and source port (9876)
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(src_port);
    src_addr.sin_addr.s_addr = inet_addr(client_ip); 

    if (bind(sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) == -1) {
        perror("[ERROR] Binding source port failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("[DEBUG] Source IP: %s, Source Port: %d bound successfully.\n", client_ip, src_port);

    // Set up destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dst_port);
    inet_pton(AF_INET, server_ip, &dest_addr.sin_addr);

    printf("[DEBUG] Sending %d %s entropy packets to %s:%d...\n", 
           num_packets, entropy ? "HIGH" : "LOW", server_ip, dst_port);

    // Send UDP packets
    for (int i = 0; i < num_packets; i++) {
        memset(buffer, 0, PACKET_SIZE);

        // Packet ID (First 2 bytes)
        buffer[0] = (i >> 8) & 0xFF;  // High byte
        buffer[1] = i & 0xFF;         // Low byte

        // Fill the payload with low (zeros) or high (random) entropy data
        if (entropy) {
            for (int j = 2; j < packet_size; j++) {
                buffer[j] = rand() % 256;
            }
        } else {
            memset(buffer + 2, 0, packet_size - 2);
        }

        // Send packet
        if (sendto(sock, buffer, packet_size, 0, 
                   (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
            perror("[ERROR] Packet send failed");
        } else {
            printf("[DEBUG] Sent packet ID %d to %s:%d (%d bytes)\n",
                   i, server_ip, dst_port, packet_size);
        }
    }

    printf("[DEBUG] Finished sending %s entropy packets.\n", entropy ? "HIGH" : "LOW");

    close(sock);
}