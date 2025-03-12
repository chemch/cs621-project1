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

}