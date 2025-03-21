#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <bits/time.h>


#define TH_SYN 0x02 

struct ipheader {
	unsigned char iph_l:4, iph_v:4;
	unsigned char iph_tos;
	unsigned short int iph_tl, iph_id, iph_offset;
	unsigned char iph_ttl, iph_proto;
	unsigned short int iph_sum;
	unsigned int iph_src, iph_dst;
};


struct tcpheader {
	unsigned short int th_sport;
	unsigned short int th_dport;
	unsigned int th_seq;
	unsigned int th_ack;
	unsigned char th_x2:4, th_off:4;
	unsigned char th_flags;
	unsigned short int th_win;
	unsigned short int th_sum;
	unsigned short int th_urp;
};

struct pseudo_header {
	unsigned int source_address;
	unsigned int dest_address;
	unsigned char placeholder;
	unsigned char protocol;
	unsigned short int tcp_length;
};


int send_syn(const char *src_ip, const char *dst_ip, int dst_port);
unsigned short checksum(const char *buf, unsigned int size);
int capture_rst(int head_port_raw, struct timeval *rst_timestamp);
