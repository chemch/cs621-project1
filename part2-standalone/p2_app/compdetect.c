# include "compdetect_alone.h"

int main(const int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "CORRECT USAGE: %s <CONFIG.JSON>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Configuration configuration = read_configuration(argv[1]);

    // validate that server ip address and port are valid
    if (configuration.server_ip[0] == '\0') {
        fprintf(stderr, "INVALID SERVER CONFIGURATION FILE PROVIDED.\n");
        exit(EXIT_FAILURE);
    }

    struct timeval head_rst_time, tail_rst_time;

    // Send Head SYN
    // send_syn(src_ip, dst_ip, syn_port_x, syn_port_x);

    // Send UDP Packet Train
    // send_udp_train(configuration);

    // Send Tail SYN
    // send_syn(src_ip, dst_ip, syn_port_y, syn_port_y);

    // // Capture RSTs
    // int got_head_rst = capture_rst(syn_port_x, &head_rst_time);
    // int got_tail_rst = capture_rst(syn_port_y, &tail_rst_time);

    // if (!got_head_rst || !got_tail_rst) {
    //     printf("Failed to detect due to insufficient information.\n");
    //     return 0;
    // }

    // double delta = (tail_rst_time.tv_sec - head_rst_time.tv_sec)
    //     + (tail_rst_time.tv_usec - head_rst_time.tv_usec) / 1e6;

    // printf("RST Arrival Delta: %.6f seconds\n", delta);

    // if (delta > COMPRESSION_THRESHOLD)
    //     printf("Compression Detected!\n");
    // else
    //     printf("No Compression Detected.\n");

    return EXIT_SUCCESS;
}