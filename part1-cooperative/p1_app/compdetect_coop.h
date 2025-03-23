#ifndef COMPDETECT_COOP_H
#define COMPDETECT_COOP_H

// ============================================
// Compression Detection Tool - Cooperative Mode
// Common Definitions and Application Defaults
// ============================================

// -----------------------------
// Standard C Library Includes
// -----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

// -----------------------------
// Networking and Socket Includes
// -----------------------------
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/udp.h>

// -----------------------------
// Project-Specific Module Includes
// -----------------------------
#include "p1_json/cJSON.h"
#include "p1_configuration/cConfig.h"
#include "p1_transport/cUDP.h"
#include "p1_transport/cTCP.h"
#include "p1_error/cError.h"

// -----------------------------
// Application Default Constants
// -----------------------------
#define DEF_UDP_SRC_PORT        9876
#define DEF_UDP_DST_PORT        8765
#define DEF_TCP_SYN_X          9999
#define DEF_TCP_SYN_Y          8888
#define DEF_TCP_PRE_PROBE      7777
#define DEF_TCP_POST_PROBE     6666
#define DEF_UDP_PAYLOAD_SIZE   1000
#define DEF_INTER_MEASURE_TIME   15
#define DEF_UDP_PACKET_COUNT   6000
#define DEF_UDP_TTL            255
#define DEF_PACK_SIZE          1024
#define DEF_BUFFER_SIZE        4096  // Buffer size for network communication

#endif // COMPDETECT_COOP_H