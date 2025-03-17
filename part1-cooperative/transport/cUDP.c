#include "cUDP.h"

void transmit_udp_train(const char *client_ip, const char *server_ip, int src_port, int dst_port, int num_packets, int packet_size, int entropy, int debug_mode) {
    int sock;
    struct sockaddr_in src_addr, dest_addr;
    char buffer[PACKET_SIZE];

    // create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("FAILED TO CREATE SOCKET.");
        exit(EXIT_FAILURE);
    }

    printf("SOCKET SET UP SUCCESSFULLY.\n");

    // set dont fragment bit to 1
    int dont_fragment_bit = IP_PMTUDISC_DO;
    if (setsockopt(sock, IPPROTO_IP, IP_MTU_DISCOVER, &dont_fragment_bit, sizeof(dont_fragment_bit)) == -1) {
        perror("FAILED TO SET DONT FRAGMENT BIT.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("DONT FRAGMENT BIT SET SUCCESSFULLY.\n");

    // bind to the specified source ip and source port
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(src_port);
    src_addr.sin_addr.s_addr = inet_addr(client_ip); 

    if (bind(sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) == -1) {
        perror("[ERROR] Binding source port failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("SOURCE IP: %s, SOURCE PORT: %d BOUND SUCCESSFULLY.\n", client_ip, src_port);

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
        memset(buffer, 0, PACKET_SIZE);

        // set packet id in the first two bytes
        buffer[0] = (i >> 8) & 0xFF;
        buffer[1] = i & 0xFF; 

        // fill the payload with low (zeros) or high (random) entropy data
        if (entropy) {
            for (int j = 2; j < packet_size; j++) {
                buffer[j] = rand() % 256;
            }
        } else {
            memset(buffer + 2, 0, packet_size - 2);
        }

        // transmit the packet
        if (sendto(sock, buffer, packet_size, 0, 
                   (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
            perror("FAILED TO SEND PACKET. ");
        } else {
            printf("PACKET ID %d SENT TO %s:%d (%d BYTES)\n",
                   i, server_ip, dst_port, packet_size);
        }
    }

    printf("SENT %s ENTROPY PACKETS SUCCESSFULLY.\n", entropy ? "HIGH" : "LOW");

    close(sock);
}
