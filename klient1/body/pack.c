#include "../headers/pack.h"

head_pack pack_header(uint32_t num, packet_type type, unsigned msg_id)
{
    head_pack header;

    header.type = type;
    header.num = num;
    header.msg_id = msg_id;

    return header;
}

packet* convert_to_packets(char *buffer, unsigned buff_len, packet_type type, unsigned msg_id)
{
    if (buffer == NULL)
        return NULL;

    unsigned amount_pack = (buff_len + 1) * sizeof(char) / PACKET_INFO +
                            ((buff_len * sizeof(char) % PACKET_INFO > 0)? 1 : 0);

    packet *packets = (packet *) malloc(sizeof(packet) * amount_pack);
    if (packets == NULL) return NULL;

    size_t current_sym_amount = 0;
    unsigned min = 0;
    for (unsigned i = 0; i < amount_pack; ++i) {
        min = MIN(PACKET_SYM_LOT, buff_len - current_sym_amount);
        memcpy(packets[i].msg, buffer + current_sym_amount, min);
        if (min != PACKET_SYM_LOT)
            buffer[min] = '\0';

        current_sym_amount += min;

        if (amount_pack == 1)
            packets[i].header = pack_header(i, type, msg_id);
        else {
            if (i == 0)
                packets[i].header = pack_header(amount_pack, QUERY, msg_id);
            else
                packets[i].header = pack_header(i, MULT_MSG, msg_id);
        }
    }
    if (min < PACKET_SYM_LOT)
        packets[amount_pack - 1].msg[min] = '\0';

    return packets;
}

packet* copy_of_packet(packet *pack) {
    packet *new = (packet *) malloc(sizeof(packet));
    if (new == NULL) {
        perror("can't allocate memory");
        return NULL;
    }
    unsigned len = strlen(pack->msg);
    if (len > PACKET_SYM_LOT) len = PACKET_SYM_LOT;
    memcpy(new->msg, pack->msg, len);
    if (len != PACKET_SYM_LOT)
        new->msg[len] = '\0';
    new->header = pack->header;

    return new;
}