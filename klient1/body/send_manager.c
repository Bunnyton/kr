#include "../headers/send_manager.h"

char* read_from_keyboard()//Дописать как в мессенджере
{
    char* buf;
    scanf(" %s", buf);
    return buf;
}

bool add_in_wait_queue(packet *pack, struct sockaddr_in *addr, deliver_status status)
{
    wait_packet_list *new = (wait_packet_list*) malloc(sizeof(wait_packet_list));
    if (new == NULL) return false;

    new->pack = pack;
    new->addr = addr;
    new->next = NULL;

    new->status = status;
    new->send_time = clock();

    if (!last) {
        start = new;
        last = new;
    } else
        last->next = new;

    return true;
}

void delete_wait_pack(wait_packet_list *wait_pack) {
    free(wait_pack->pack);
    free(wait_pack->addr);
    free(wait_pack);
}

double diff_time(time_t time) { return (double)(clock() - time) / CLOCKS_PER_SEC; }

bool check_wait_queue(NetworkContext *ctx)
{
    wait_packet_list *current = start;
    wait_packet_list *previous = start;
    while (current != NULL){
        if (current->status == DELIVERED) {
            if (previous == current) {
                start = current->next;
                delete_wait_pack(current);
                current = start;
            } else {
                previous->next = current->next;
                delete_wait_pack(current);
                current = previous;
            }
            if (current->next == NULL)
                last = current;
        } else {
            double wait_time = WAIT_TIME_FOR_SEND;
            if (current->status == WAIT_SIGNAL)
                wait_time = WAIT_TIME_FOR_SIGNAL;
            if (current->status == ERROR)
                wait_time = 0;

            if (diff_time(current->send_time) >= wait_time) {
                if (!send_packet(ctx, current->pack, current->addr))
                    return false;
                current->send_time = clock();
            }
        }
        previous = current;
        current = current->next;
    }
    return true;
}

bool send_packet(NetworkContext *ctx, packet *pack, struct sockaddr_in *addr)
{
    if (!add_in_wait_queue(pack, addr, WAIT_DELIVER))
        return false;

    ssize_t n = sendto(ctx->sock, pack
            , sizeof(packet)
            , 0
            , (const struct sockaddr *) addr
            , sizeof(*addr));

    if (n != sizeof(packet)) {
        last->status = ERROR;
        return false;
    }

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
    bool status = send_msg_to(buffer, buff_len, *addr);

    free(addr);
    return  status;
}

bool send_signal(NetworkContext *ctx, int16_t id, struct sockaddr_in *addr)
{
    packet pack;
    pack.header = pack_header(0, id, SIGNAL);

    return send_packet(ctx, &pack, addr);
}

