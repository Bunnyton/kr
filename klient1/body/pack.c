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

packet* convert_to_packets(char *buffer, unsigned buff_len, uint16_t rec_id, packet_type type)
{
    if (buffer == NULL)
        return NULL;

    unsigned amount_pack = (buff_len + 1) * sizeof(char) / PACKET_INFO +
                            ((buff_len * sizeof(char) % PACKET_INFO > 0)? 1 : 0);

    packet *packets = (packet *) malloc(sizeof(packet) * amount_pack);
    if (packets == NULL) return NULL;

    size_t current_size = 0;
    for (unsigned i = 0; i < amount_pack; ++i) {
        current_size = strlen(buffer) * sizeof(char) - PACKET_INFO * i;
        memcpy(packets[i].msg, buffer + current_size, MIN(PACKET_INFO, current_size));

        if (amount_pack == 1)
            packets[i].header = pack_header(i, rec_id, type);
        else {
            if (i == 0)
                packets[i].header = pack_header(amount_pack, rec_id, QUERY);
            else
                packets[i].header = pack_header(i, rec_id, MULT_MSG);
        }
    }
    if (current_size < PACKET_INFO)
        packets[amount_pack - 1].msg[current_size] = '\0';

    return packets;
}
