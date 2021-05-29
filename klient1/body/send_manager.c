#include "../headers/send_manager.h"

char* read_from_keyboard()//Дописать как в мессенджере
{
    char* buf;
    scanf(" %s", buf);
    return buf;
}

bool send_packet(NetworkContext *ctx, packet *pack, struct sockaddr_in *addr)
{
    ssize_t n = sendto(ctx->sock, pack
            , sizeof(packet)
            , 0
            , (const struct sockaddr *) addr
            , sizeof(*addr));

    if (n != sizeof(packet))
        return false;

    return true;
}

bool send_msg_to(char* buffer, unsigned buff_len, in_addr_t addr)
{
    if (buff_len == 0)
        return false;//FIXME добавь виды ошибок убери bool

    NetworkContext *ctx = network_init(PORT, addr);
    if (!ctx) return false;

    packet *packets = convert_to_packets(buffer, buff_len, UNKNOWN, MSG);

    if (!send_packet(ctx, &packets[0], &ctx->addr)){
        free(packets);
        network_fini(ctx);
        return false;
    }

    if (packets[0].header.type == QUERY) {
        sleep(5);//FIXME Ждать сигнал

        for (int i = 1; i < packets[0].header.num; ++i)
            if (!send_packet(ctx, &packets[i], &ctx->addr)) {
                network_fini(ctx);
                return false;
            }

    }
    network_fini(ctx);
    return true;
}

bool send_msg(char* buffer, unsigned buff_len, uint16_t id)
{
    in_addr_t *addr = find_addr(id);
    if (addr == NULL) {
        return false;
        //Возможно, здесь может быть что-то завязанное на регистрацию
    }
    return send_msg_to(buffer, buff_len, *addr);
}

bool send_signal(NetworkContext *ctx, int16_t id, struct sockaddr_in *addr)
{
    packet pack;
    pack.header = pack_header(0, id, SIGNAL);

    return send_packet(ctx, &pack, addr);
}

