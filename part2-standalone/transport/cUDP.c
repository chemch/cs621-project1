// #include "cUDP.h"

// void send_udp_train(const Configuration configuration) {
//     // extract configuration parameters
//     char *src_ip = configuration.client_ip;
//     char *dst_ip = configuration.server_ip;
//     int src_port = configuration.udp_src_port;
//     int dst_port = configuration.udp_dst_port;
//     int udp_payload_size = configuration.udp_payload_size;
//     int n_packets = configuration.udp_packet_count;
//     int ttl = configuration.ttl;

//     char packet[PACKET_SIZE];
//     struct iphdr *ip = (struct iphdr *) packet;
//     struct udphdr *udp = (struct udphdr *) (packet + sizeof(struct iphdr));
//     char *data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);

//     int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
//     if (sock < 0) { perror("socket"); exit(EXIT_FAILURE); }

//     int one = 1;
//     setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));

//     for (int i = 0; i < n_packets; i++) {
//         memset(packet, 0, PACKET_SIZE);

//         // Fill Payload (zeros or random if needed)
//         memset(data, 0, udp_payload_size);
//         data[0] = (i >> 8) & 0xFF;
//         data[1] = i & 0xFF;

//         // IP Header
//         ip->ihl = 5;
//         ip->version = 4;
//         ip->tos = 0;
//         ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + udp_payload_size);
//         ip->id = htons(54321 + i);
//         ip->frag_off = htons(0x4000);
//         ip->ttl = ttl;
//         ip->protocol = IPPROTO_UDP;
//         ip->saddr = inet_addr(src_ip);
//         ip->daddr = inet_addr(dst_ip);

//         // UDP Header
//         udp->source = htons(src_port);
//         udp->dest = htons(dst_port);
//         udp->len = htons(sizeof(struct udphdr) + udp_payload_size);
//         udp->check = 0;

//         struct sockaddr_in dest;
//         dest.sin_family = AF_INET;
//         dest.sin_port = udp->dest;
//         dest.sin_addr.s_addr = ip->daddr;

//         sendto(sock, packet, ntohs(ip->tot_len), 0, (struct sockaddr *)&dest, sizeof(dest));
//         sleep(500);  // Small delay
//     }

//     close(sock);
// }
