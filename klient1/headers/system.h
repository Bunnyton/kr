#pragma once

#include "network.h"
#include "send_manager.h"
#include "receive_manager.h"
#include "processing_manager.h"

bool sign_up();

NetworkContext* system_init();

void system_start(NetworkContext *ctx);