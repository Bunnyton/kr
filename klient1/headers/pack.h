#pragma once

#define PACKET_SYM_LOT 128
#define PACK_STR_INFO 4
#define PACK_HEAD_STR_LOT 4

#define PACKET_INFO (PACKET_SYM_LOT * sizeof(char))
#define PACKET_SIZE (PACKET_INFO + PACK_HEAD_STR_LOT * PACK_STR_INFO)

#include <sys/param.h>

#include "header.h"

enum _packet_type {
      MSG
    , MULT_MSG
    , QUERY
    , SIGNAL
    , SIGN
};

typedef enum _packet_type packet_type;

struct _head_pack {
    packet_type type;
    uint32_t num;
    unsigned msg_id;
};

typedef struct _head_pack head_pack;

struct _packet {
    head_pack header;
    char msg[PACKET_SYM_LOT];//максимальное количество символов в одном пакете - 128
};

typedef struct _packet packet;

head_pack pack_header(uint32_t num, packet_type type, unsigned msg_id);

packet* convert_to_packets(char *buffer, unsigned buff_len, packet_type type, unsigned msg_id);
//Если тип пакета соответствует MSG, то он может быть изменен на MULT_MSG и Query

packet* copy_of_packet(packet *pack);