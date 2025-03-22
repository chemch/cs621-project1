#ifndef P2_COMPDETECT_H
#define P2_COMPDETECT_H

// ===================================================
// Project 2 - Standalone Compression Detection Tool
// Common Definitions and Application Defaults
// ===================================================

// -----------------------------
// Standard C Library Includes
// -----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>      
#include <stddef.h>      
#include <stdbool.h>     
#include <errno.h>

// -----------------------------
// Networking and Socket Includes
// -----------------------------
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netdb.h>

// -----------------------------
// Multithreading
// -----------------------------
#include <pthread.h>

// -----------------------------
// Project-Specific Module Headers
// -----------------------------
#include "p2_json/cJSON.h"
#include "p2_configuration/cConfig.h"
#include "p2_transport/cUDP.h"
#include "p2_transport/cRawTCP.h"
#include "p2_flow/cFlow.h"
#include "p2_error/cError.h"

// -----------------------------
// Application Default Values
// -----------------------------
#define DEF_UDP_SRC_PORT         9876
#define DEF_UDP_DST_PORT         8765
#define DEF_TCP_SYN_X            9999
#define DEF_TCP_SYN_Y            8888
#define DEF_UDP_PAYLOAD_SIZE     1000
#define DEF_UDP_PACKET_COUNT     6000
#define DEF_INTER_MEASURE_TIME   15
#define DEF_UDP_TTL              255
#define DEF_COMPRESION_THRES     0.1
#define DEF_BUFFER_SIZE          4096
#define DEF_PACK_SIZE            1024
#define DEF_MAX_PACKET_SIZE      4096
#define DEF_TCP_SYN_FLAG         0x02 // SYN flag for TCP handshake

#endif // P2_COMPDETECT_H
