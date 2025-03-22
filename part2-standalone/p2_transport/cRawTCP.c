#include "cRawTCP.h"

#define MAX_PACKET_SIZE 4096
#define SOCKET_TIMEOUT_SEC 15
#define RST_LISTEN_TIMEOUT_SEC 60
#define SEC_TO_MS 1000
#define NS_TO_MS 1000000

// Generates and sends a single TCP SYN packet
int send_syn(const char *src_ip, const char *dst_ip, int dst_port) {
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (fd == -1) {
        perror("FAILED TO OPEN SOCKET");
        return -1;
    }

    // Random ephemeral port [32768, 60999]
    int ephemeral_port = 32768 + rand() % (60999 - 32768 + 1);
    int th_sport = htons(ephemeral_port);

    char packet[MAX_PACKET_SIZE];
    memset(packet, 0, sizeof(packet));

    struct ipheader *iphdr = (struct ipheader *) packet;
    struct tcpheader *tcphdr = (struct tcpheader *) (packet + sizeof(struct ipheader));
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(dst_ip);

    // IP header setup
    iphdr->iph_l = 5;
    iphdr->iph_v = 4;
    iphdr->iph_tos = 0;
    iphdr->iph_tl = htons(sizeof(struct ipheader) + sizeof(struct tcpheader));
    iphdr->iph_id = htons(0);
    iphdr->iph_offset = 0;
    iphdr->iph_ttl = 255;
    iphdr->iph_proto = IPPROTO_TCP;
    iphdr->iph_sum = 0;
    iphdr->iph_src = inet_addr(src_ip);
    iphdr->iph_dst = sin.sin_addr.s_addr;

    // TCP header setup
    tcphdr->th_sport = th_sport;
    tcphdr->th_dport = htons(dst_port);
    tcphdr->th_seq = rand();
    tcphdr->th_ack = 0;
    tcphdr->th_x2 = 0;
    tcphdr->th_off = 5;
    tcphdr->th_flags = TH_SYN;
    tcphdr->th_win = htons(65535);
    tcphdr->th_sum = 0;
    tcphdr->th_urp = 0;

    // Pseudo-header for checksum
    struct pseudo_header psh = {
        .source_address = iphdr->iph_src,
        .dest_address = iphdr->iph_dst,
        .placeholder = 0,
        .protocol = IPPROTO_TCP,
        .tcp_length = htons(sizeof(struct tcpheader))
    };

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcpheader);
    char *pseudogram = malloc(psize);
    if (!pseudogram) {
        perror("FAILED TO ALLOCATE MEMORY FOR PSEUDOGRAM");
        close(fd);
        return -1;
    }

    memcpy(pseudogram, &psh, sizeof(psh));
    memcpy(pseudogram + sizeof(psh), tcphdr, sizeof(struct tcpheader));

    // Checksums
    tcphdr->th_sum = checksum(pseudogram, psize);
    iphdr->iph_sum = checksum((const char*) packet, ntohs(iphdr->iph_tl));

    free(pseudogram);

    // Include IP header
    int one = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("FAILED TO SET IP_HDRINCL");
        close(fd);
        return -1;
    }

    if (sendto(fd, packet, ntohs(iphdr->iph_tl), 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("FAILED TO SEND PACKET");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// Calculates checksum for IP/TCP headers
unsigned short checksum(const char *buf, unsigned int size) {
    unsigned long sum = 0;
    for (unsigned int i = 0; i < size - 1; i += 2) {
        unsigned short word = *(unsigned short *)&buf[i];
        sum += word;
    }
    if (size & 1) {
        sum += (unsigned char)buf[size - 1];
    }
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return (unsigned short)(~sum);
}

// Listens for RST packet and records timestamp
int capture_rst(int target_port, struct timeval *rst_timestamp) {
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (fd < 0) {
        perror("FAILED TO CREATE SOCKET");
        return 0;
    }

    // Set timeout for recvfrom
    struct timeval timeout = {SOCKET_TIMEOUT_SEC, 0};
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("FAILED TO SET SOCKET TIMEOUT");
        close(fd);
        return 0;
    }

    char buffer[MAX_PACKET_SIZE];
    struct sockaddr_storage sender;
    socklen_t sender_len = sizeof(sender);

    struct timeval start, now;
    gettimeofday(&start, NULL);

    printf("LISTENING FOR RST PACKETS ON PORT %d...\n", target_port);

    while (1) {
        gettimeofday(&now, NULL);
        if (now.tv_sec - start.tv_sec >= RST_LISTEN_TIMEOUT_SEC) {
            printf("TIMEOUT WAITING FOR RST.\n");
            break;
        }

        int recvd = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender, &sender_len);
        if (recvd > 0) {
            if ((unsigned char)buffer[9] == IPPROTO_TCP && (buffer[33] & 0x04)) {
                unsigned short port = (buffer[20] << 8) | buffer[21];
                if (port == target_port) {
                    if (gettimeofday(rst_timestamp, NULL) != 0) {
                        perror("FAILED TO GET RST TIMESTAMP");
                        close(fd);
                        return 0;
                    }
                    printf("RECEIVED RST FROM PORT %d AT %ld.%06ld\n", target_port, rst_timestamp->tv_sec, rst_timestamp->tv_usec);
                    close(fd);
                    return 1;
                }
            }
        }
    }

    close(fd);
    return 0;
}
