#include "cRawTCP.h"

/// @brief Calculates the checksum of a buffer
/// @param buf buffer to calculate checksum
/// @param size size of buffer
/// @return  checksum
unsigned short computeChecksum(const char *buf, unsigned int size) {
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


/// @brief send_syn sends a raw TCP SYN packet
/// @param src_ip  source IP address
/// @param dst_ip destination IP address
/// @param dst_port destination port number
/// @return result of sending SYN packet
int tcp_syn_transmission(const char *src_ip, const char *dst_ip, int dst_port) {

    // Open raw socket
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (fd == -1) {
        perror("FAILED TO OPEN SOCKET");
        return -1;
    }

    // Random ephemeral port (for source port)
    int ephemeral_port = 32768 + rand() % (60999 - 32768 + 1);
    int th_sport = htons(ephemeral_port);

    // Packet buffer
    char packet[DEF_MAX_PACKET_SIZE];
    memset(packet, 0, sizeof(packet));

    // IP and TCP headers
    struct ipheader *iphdr = (struct ipheader *) packet;
    struct tcpheader *tcphdr = (struct tcpheader *) (packet + sizeof(struct ipheader));
    struct sockaddr_in sin;

    // Destination IP setup
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(dst_ip);

    // IP header setup
    iphdr->iph_v = 4;
    iphdr->iph_tos = 0;
    iphdr->iph_l = 5;
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

    // Pseudo Header setup
    int psize = sizeof(struct pseudo_header) + sizeof(struct tcpheader);
    char *psudeo_header = malloc(psize);
    if (!psudeo_header) {
        perror("FAILED TO ALLOCATE MEMORY FOR PSEUDOGRAM");
        close(fd);
        return -1;
    }

    // Copy headers to pseudogram
    memcpy(psudeo_header, &psh, sizeof(psh));
    memcpy(psudeo_header + sizeof(psh), tcphdr, sizeof(struct tcpheader));

    // Checksums
    tcphdr->th_sum = computeChecksum(psudeo_header, psize);
    iphdr->iph_sum = computeChecksum((const char*) packet, ntohs(iphdr->iph_tl));

    free(psudeo_header);

    // Include IP header
    int one = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("FAILED TO SET IP HEADER INCLUSION");
        close(fd);
        return -1;
    }

    // Send packet
    if (sendto(fd, packet, ntohs(iphdr->iph_tl), 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("FAILED TO SEND PACKET");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

/// @brief capture_rst listens for a RST packet on a specified port
/// @param target_port target port to listen for RST
/// @param rst_timestamp timestamp of RST packet
/// @return capture success
int record_reset_packet(int target_port, struct timeval *rst_timestamp, int reset_timeout, int socket_timeout, int debug_mode) {

    // Open raw socket
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (fd < 0) {
        perror("FAILED TO CREATE SOCKET");
        return 0;
    }

    // Set timeout for recvfrom
    struct timeval timeout = {socket_timeout, 0};
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("FAILED TO SET SOCKET TIMEOUT");
        close(fd);
        return 0;
    }

    // Bind to any IP address
    char buffer[DEF_MAX_PACKET_SIZE];
    struct sockaddr_storage sender;
    socklen_t sender_len = sizeof(sender);

    // Start timer
    struct timeval start, now;
    gettimeofday(&start, NULL);

    if (debug_mode) {
        printf("LISTENING FOR RST PACKETS ON PORT %d...\n", target_port);
    }

    // Listen for RST packets on infinite loop
    while (1) {
        gettimeofday(&now, NULL);
        if (now.tv_sec - start.tv_sec >= reset_timeout) {
            printf("TIMEOUT WAITING FOR RST.\n");
            break;
        }

        // Receive packet
        int recvd = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender, &sender_len);
        if (recvd > 0) {

            // Check for RST flag
            if ((unsigned char)buffer[9] == IPPROTO_TCP && (buffer[33] & 0x04)) {
                unsigned short port = (buffer[20] << 8) | buffer[21];

                // confirm IP packet has the same port as the target port
                if (port == target_port) {
                    if (gettimeofday(rst_timestamp, NULL) != 0) {
                        perror("FAILED TO GET RST TIMESTAMP");
                        close(fd);
                        return 0;
                    }

                    // Print RST timestamp
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
