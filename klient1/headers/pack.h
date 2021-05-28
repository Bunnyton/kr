#pragma once
#define KLIENT1_PACK_H

//--------------------------------------------ПАКЕТ---------------------------------------------------------------------
//|   БИТЫ   |    0 - 2     |    3 -  15      |      16 - 31      |
//|    0     |       ID ОТПРАВИТЕЛЯ           |   ID ПОЛУЧАТЕЛЯ   |
//|   32     |                  НОМЕР ПАКЕТА                      |
//|   64     |             НОМЕР СЛЕДУЮЩЕГО ПАКЕТА                |
//|   96     | ТИП ПАКЕТА  |  ЗАРЕЗЕРВИРОВАНО | КОНТРОЛЬНАЯ СУММА |
//| 128-2176 |                    ДАННЫЕ                          |
//---------------------------------------------------------------------------------------------------------------------

#define PACKET_SYM_LOT 128
#define PACK_STR_INFO 4
#define PACK_HEAD_STR_LOT 4

#define PACKET_INFO (PACKET_SYM_LOT * sizeof(char))
#define PACKET_SIZE (PACKET_INFO + PACK_HEAD_STR_LOT * PACK_STR_INFO)


#include "header.h"

enum _packet_type {
      MSG
    , MULT_MSG
    , QUERY
    , SIGNAL
    , SIGN
    , SIGN_OK
};

typedef enum _packet_type packet_type;

struct _head_pack {
    uint16_t send_id;
    uint16_t rec_id;
    packet_type type;
    uint32_t num;
};

typedef struct _head_pack head_pack;

struct _packet {
    head_pack header;
    char msg[PACKET_SYM_LOT];//максимальное количество символов в одном пакете - 128
};

typedef struct _packet packet;

head_pack pack_header(uint32_t num, uint16_t rec_id, packet_type type);

packet* convert_to_packets(char *buffer, int buff_len, uint16_t rec_id, packet_type type);
//Если тип пакета соответствует MSG, то он может быть изменен на MULT_MSG и Query