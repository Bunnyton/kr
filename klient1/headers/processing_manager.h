#pragma once

#include "network.h"
#include "filework.h"
#include "send_manager.h"

struct _mult_msg{
    unsigned amount;
    packet_list **packets;
    struct _mult_msg *next;
};
typedef struct _mult_msg mult_msg;

mult_msg *start_mult_msg;
mult_msg *last_mult_msg;

bool make_new_mult_msg(packet_list *pack_list);
void add_in_mult_msg(packet_list *pack_list);

bool add_user_rec(uint16_t id, char *nick, in_addr_t addr);

void process(packet_list *pack_list);

void* start_processing_manager();
