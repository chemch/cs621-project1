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

int main(void) {
    printf("Hello Client\n");
    return 0;
}
