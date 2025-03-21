#ifndef COMPDETECT_H
#define COMPDETECT_H

// required external dependencies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <netinet/ip.h>     
#include <netinet/in.h>
#include <netinet/udp.h>    
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/time.h>


// required local dependencies
#include "p2_json/cJSON.h"
#include "p2_configuration/cConfig.h"
#include "p2_transport/cUDP.h"
#include "p2_transport/cRawTCP.h"
#include "p2_flow/cFlow.h"

// application defaults (per project 1 pdf instructions)
#define DEF_UDP_SRC_PORT 9876
#define DEF_UDP_DST_PORT 8765
#define DEF_TCP_SYN_X 9999
#define DEF_TCP_SYN_Y 8888
#define DEF_UDP_PAYLOAD_SIZE 1000
#define DEF_UDP_PACKET_COUNT 6000
#define DEF_INTER_MEASURE_TIME 15
#define DEF_UDP_TTL 255

// buffer size for communication between client and server
#define DEF_BUFFER_SIZE 4096

// packet size for UDP packets
#define PACKET_SIZE 1024 

#endif
