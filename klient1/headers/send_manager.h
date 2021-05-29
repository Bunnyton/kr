#pragma  once

#include "network.h"
#include "filework.h"
#include "str_addit_methods.h"

char* read_from_keyboard();
bool send_packet(NetworkContext *ctx, packet *pack, struct sockaddr_in *addr);
bool send_msg_to(char *buffer, unsigned buffer_len, in_addr_t addr);
bool send_msg(char *buffer, unsigned buffer_len, uint16_t id);
