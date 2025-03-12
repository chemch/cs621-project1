#include "compdetect_udp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <time.h>
#include "compdetect.h"
#include <netinet/ip.h>     // Provides struct iphdr

// Function to calculate checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}


void send_udp_packets(const char *server_ip, int src_port, int dst_port, int num_packets, int packet_size, int entropy) {
    int sock;
    struct sockaddr_in dest_addr;
    char *packet;
    struct iphdr *ip_header;
    struct udphdr *udp_header;
    struct pseudo_header psh;

    // Create raw socket
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("Raw socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Enable IP header inclusion
    int optval = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) == -1) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the packet
    packet = malloc(sizeof(struct iphdr) + sizeof(struct udphdr) + packet_size);
    if (!packet) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Set up destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dst_port);
    inet_pton(AF_INET, server_ip, &dest_addr.sin_addr);

    srand(time(NULL)); // Initialize random seed

    for (int i = 0; i < num_packets; i++) {
        memset(packet, 0, sizeof(struct iphdr) + sizeof(struct udphdr) + packet_size);

        // Construct IP header
        ip_header = (struct iphdr *)packet;
        ip_header->ihl = 5;  // Internet Header Length (5 * 4 = 20 bytes)
        ip_header->version = 4;
        ip_header->tos = 0;
        ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + packet_size);
        ip_header->id = htons(i);  // Unique packet ID
        ip_header->frag_off = htons(0x4000);  // **Set DF bit**
        ip_header->ttl = 255;
        ip_header->protocol = IPPROTO_UDP;
        ip_header->saddr = inet_addr("192.168.1.2");  // Set your actual source IP
        ip_header->daddr = dest_addr.sin_addr.s_addr;
        ip_header->check = checksum(ip_header, sizeof(struct iphdr));

        // Construct UDP header
        udp_header = (struct udphdr *)(packet + sizeof(struct iphdr));
        udp_header->source = htons(src_port);
        udp_header->dest = htons(dst_port);
        udp_header->len = htons(sizeof(struct udphdr) + packet_size);
        udp_header->check = 0;  // Leave checksum calculation for later

        // Construct pseudo header for checksum calculation
        psh.source_address = ip_header->saddr;
        psh.dest_address = ip_header->daddr;
        psh.placeholder = 0;
        psh.protocol = IPPROTO_UDP;
        psh.udp_length = udp_header->len;

        // Copy pseudo header + UDP header + payload for checksum calculation
        char *checksum_buffer = malloc(sizeof(struct pseudo_header) + sizeof(struct udphdr) + packet_size);
        memcpy(checksum_buffer, &psh, sizeof(struct pseudo_header));
        memcpy(checksum_buffer + sizeof(struct pseudo_header), udp_header, sizeof(struct udphdr) + packet_size);
        udp_header->check = checksum(checksum_buffer, sizeof(struct pseudo_header) + sizeof(struct udphdr) + packet_size);
        free(checksum_buffer);

        // Set payload data (First 2 bytes = Packet ID)
        char *payload = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
        payload[0] = (i >> 8) & 0xFF;  // High byte of packet ID
        payload[1] = i & 0xFF;         // Low byte of packet ID
        memset(payload + 2, entropy ? rand() % 256 : 0, packet_size - 2);  // Fill remaining payload

        // Send the UDP packet
        if (sendto(sock, packet, ntohs(ip_header->tot_len), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
            perror("Packet send failed");
        }
    }

    printf("Sent %d %s entropy UDP packets to %s:%d with DF bit set\n",
           num_packets, entropy == 0 ? "LOW" : "HIGH", server_ip, dst_port);

    free(packet);
    close(sock);
}