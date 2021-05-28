#include "../headers/pack.h"

head_pack pack_header(uint32_t num, uint16_t rec_id, packet_type type)
{
    head_pack header;

    header.send_id = ID;
    header.rec_id = rec_id;
    header.type = type;
    header.num = num;

    return header;
}

packet* convert_to_packets(char *buffer, int buff_len, uint16_t rec_id, packet_type type)
{
    if (buffer == NULL)
        return NULL;

    unsigned amount_pack = (buff_len + 1) * sizeof(char) / PACKET_INFO +
                            ((buff_len * sizeof(char) % PACKET_INFO > 0)? 1 : 0);

    packet *packets = (packet *) malloc(sizeof(packet) * amount_pack);
    if (packets == NULL) return NULL;

    unsigned pack_buff_len = buff_len % PACKET_SYM_LOT;
    packets[amount_pack - 1].msg[pack_buff_len] = '\0';

    for(unsigned i = (int) amount_pack; i > 0; --i) {

        if (amount_pack == 1)
            packets[i].header = pack_header(i, rec_id, type);
        else
            if (i == 0)
                packets[i].header = pack_header(amount_pack, rec_id, QUERY);
            else
                packets[i].header = pack_header(i, rec_id, MULT_MSG);

        for(unsigned j = pack_buff_len; j > 0; --j) {
            packets[i].msg[j] = buffer[buff_len - 1];
            --buff_len;
        }
        pack_buff_len = PACKET_SYM_LOT;
    }

    return packets;
}
