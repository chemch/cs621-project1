#include "cRawTCP.h"

#include <time.h>

#define SEC_TO_MS 1000
#define NS_TO_MS 1000000

int send_syn(const char *src_ip, const char *dst_ip, int dst_port) {
	// Open the socket
	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd == -1) {
        perror("Failed to open socket with error code:" + errno);
	}

    // pick an ephemeral random source port for tcp
    int ephemeral_port = 32768 + rand() % (60999 - 32768 + 1);
    int th_sport = htons(ephemeral_port);

	// Initialize the raw packet
	char packet[4096];
	// Initialize the header structs in the correct place in the packet
	struct ipheader *iphdr = (struct ipheader *) packet;
	struct tcpheader *tcphdr = (struct tcpheader *) (packet + sizeof(struct ipheader));
	struct sockaddr_in sin;
	struct pseudo_header psh;
	// Populate sockaddr_in struct with server information
	sin.sin_family = AF_INET;
	tcphdr->th_dport = htons(1234);
	sin.sin_addr.s_addr = inet_addr(dst_ip);
	memset(packet, 0, 4096);

	// Fill in IPv4 header
    
	iphdr->iph_l = 5;
	iphdr->iph_v = 0b100;
	iphdr->iph_tos = 0;	
	iphdr->iph_tl = sizeof(struct ipheader) + sizeof(struct tcpheader);
	iphdr->iph_id = htonl(0);
	iphdr->iph_offset = 0;
	iphdr->iph_ttl = 255;
	iphdr->iph_proto = IPPROTO_TCP;
	iphdr->iph_sum = 0;
	iphdr->iph_src = inet_addr(src_ip);
	iphdr->iph_dst = sin.sin_addr.s_addr;

	// Fill in TCP header
    tcphdr->th_sport = th_sport;
    tcphdr->th_dport = htons(dst_port); 
	tcphdr->th_seq =  rand();
	tcphdr->th_ack = 0;
	tcphdr->th_x2 = 0;
	tcphdr->th_off = 5;
	tcphdr->th_flags = TH_SYN;
	tcphdr->th_win = htons(65535);
	tcphdr->th_sum = 0;
	tcphdr->th_urp = 0;

	// Fill in pseudoheader
	psh.source_address = inet_addr(src_ip);
	psh.dest_address = sin.sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcpheader));
	int psize = sizeof(struct pseudo_header) + sizeof(struct tcpheader);

	// Populate pseudogram
	char *pseudogram = malloc(psize);
	memcpy(pseudogram, (char*)&psh, sizeof(struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header), tcphdr, sizeof(struct tcpheader));

	// Calculate header checksums: Use pseudogram for tcp header, and filled packet for the ip header
	tcphdr->th_sum = checksum((const char*) pseudogram, psize);
	iphdr->iph_sum = checksum((const char*) packet, iphdr->iph_tl);

	// Set IP_HDRINCL
	int one = 1;
	const int *val = &one;
	int err_check = setsockopt(fd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one));
	if (err_check < 0) {
		printf("Warning: Cannot set HDRINCL!\n");
	}

	// Send the packet
	err_check = sendto(fd, packet, iphdr->iph_tl, 0, (struct sockaddr *) &sin, sizeof(sin));
	if (err_check < 0) {
		printf("error: errno = %i\n", errno);
	}
	free(pseudogram);
	return fd;
}



unsigned short checksum(const char *buf, unsigned int size)
{
    unsigned sum = 0, i;

	/* Accumulate checksum */
	for (i = 0; i < size - 1; i += 2)
	{
		unsigned short word16 = *(unsigned short *) &buf[i];
		sum += word16;
	}

	/* Handle odd-sized case */
	if (size & 1)
	{
		unsigned short word16 = (unsigned char) buf[i];
		sum += word16;
	}

	/* Fold to get the ones-complement result */
	while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);

	/* Invert to get the negative in ones-complement arithmetic */
	return ~sum;
}

int capture_rst(int head_port_raw, struct timeval *rst_timestamp) {
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (fd < 0) {
        perror("Failed to create socket");
        return 0;  // failure
    }

    struct sockaddr_storage server;
    socklen_t addr_len = sizeof(server);
    char buffer[4096];

    printf("Listening for RST packets on port %d...\n", head_port_raw);

    struct timeval start_time, current_time;
    gettimeofday(&start_time, NULL);

    // Set 15s socket timeout
    struct timeval tv;
    tv.tv_sec = 15;
    tv.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        perror("Failed to set socket timeout");
        close(fd);
        return 0;
    }

    int head_received = 0;

    while (1) {
        gettimeofday(&current_time, NULL);
        if ((current_time.tv_sec - start_time.tv_sec) >= 60) {
            printf("Timeout waiting for RST.\n");
            break;
        }

        int rec = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &server, &addr_len);
        if (rec > 0) {
            // Check for TCP protocol and RST flag
            if ((unsigned char)buffer[9] == IPPROTO_TCP && (buffer[33] & 0x04)) {  // TCP and RST
                unsigned short int sport = (buffer[20] << 8) | buffer[21];

				fprintf(stderr, "Received RST: Source Port from Buffer %d\n", sport);
				fprintf(stderr, "Looking for Port Number:  %d\n", head_port_raw);

                if (sport == head_port_raw) {
                    // Record timestamp of RST packet
                    if (gettimeofday(rst_timestamp, NULL) != 0) {
                        perror("Failed to get RST timestamp");
                        close(fd);
                        return 0;
                    }
                    printf("Received RST from port %d at %ld.%06ld\n", head_port_raw, rst_timestamp->tv_sec, rst_timestamp->tv_usec);
                    head_received = 1;
                    break;
                }
            }
        }
    }

    close(fd);
    return head_received;
}
