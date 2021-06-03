#pragma once

#include "network.h"

bool receive_pack(NetworkContext *ctx, packet_list *pack_list);
void* start_receive_manager(NetworkContext *ctx);
