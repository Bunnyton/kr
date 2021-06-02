#pragma once

#include "pack.h"
#include "header.h"
#include "filework.h"

#define UNKNOWN 0
#define MY_ID 1

struct _NetworkContext {
    int sock;
    struct sockaddr_in addr;
};
typedef struct _NetworkContext NetworkContext;

struct _packet_list{
    packet *pack;
    struct sockaddr_in addr;
    uint16_t id;
    struct _packet_list *next;
};
typedef struct _packet_list packet_list;

packet_list *fifo_recv_start;
packet_list *fifo_recv_last;

bool process_permission_flag;

NetworkContext* network_init(int port, in_addr_t addr);
void network_fini(NetworkContext *ctx);

in_addr_t s_addr_of(packet_list *pack_list);

packet_list* copy_of_packet_list(packet_list *pack_list);