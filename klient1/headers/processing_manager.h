#pragma once

#include "network.h"
#include "filework.h"
#include "send_manager.h"

bool add_user_rec(uint16_t id, char *nick, in_addr_t addr);

void process_mult_msg(packet_list *pack_list);

void process(packet_list *pack_list);

char* form_mult_msg(packet_list *pack_list);

void start_processing_manager();
