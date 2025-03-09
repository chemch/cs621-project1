#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "cJSON.h"

#define DEF_UDP_SRC_PORT 9876
#define DEF_UDP_DST_PORT 8765
#define DEF_TCP_SYN_X 9999
#define DEF_TCP_SYN_Y 8888
#define DEF_TCP_PRE_PROBE 7777
#define DEF_TCP_POST_PROBE 6666
#define DEF_UDP_PAYLOAD_SIZE 1000
#define DEF_INTER_MEASURE_TIME 15
#define DEF_UDP_PACKET_COUNT 6000
#define DEF_TTL 255

/*
 * PART 1: Compression Detection Client/Server Application
*/
int main(const int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <configuration.json>\n", argv[0]);

        // exit with failure
        return EXIT_FAILURE;
    }

    // parse config file provided
    char *config_file_name= argv[1];
    fprintf(stderr, "Parsing Configuration File: %s\n", config_file_name);

    return EXIT_SUCCESS;
}
