#include "../headers/send_manager.h"

bool add_in_wait_queue(packet *pack, struct sockaddr_in *addr, deliver_status status)
{
    wait_packet_list *new = (wait_packet_list*) malloc(sizeof(wait_packet_list));
    if (new == NULL) {
        perror("can't allocate memory");
        return false;
    }

    new->pack = copy_of_packet(pack);
    if (new->pack == NULL)
        goto error1;


    new->addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    if (new->addr == NULL)
        goto error2;

    new->addr->sin_addr.s_addr = addr->sin_addr.s_addr;
    new->next = NULL;

    new->status = status;
    new->send_time = clock();

    if (!start) {
        start = new;
        last = new;
    } else {
        last->next = new;
        last = new;
    }

    return true;

    error2:
    free(new->pack);
    error1:
    free(new);
    return false;
}

wait_packet_list* find_in_wait_queue(in_addr_t addr, deliver_status status, uint8_t msg_id) {
    struct in_addr saddr;
    saddr.s_addr = addr;
    char *str_addr = inet_ntoa(saddr);

    wait_packet_list *current = start;
    char *str = inet_ntoa(current->addr->sin_addr);
    while (current != NULL && (strcmp(inet_ntoa(current->addr->sin_addr), str_addr) != 0
                                                    || current->status != status
                                                    || current->pack->header.msg_id != msg_id)) {
        current = current->next;
    }
    return current;
}

void delete_wait_pack(wait_packet_list *wait_pack) {
    free(wait_pack->pack);
    free(wait_pack->addr);
    free(wait_pack);
}

double diff_time(time_t time) { return (double)(clock() - time) / CLOCKS_PER_SEC; }

bool check_wait_queue()
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
            if (current && current->next == NULL)
                last = current;
        } else {
            if (current->status == WAIT_SEND){
                in_addr_t addr = current->addr->sin_addr.s_addr;
                current->status = WAIT_DELIVER;
                current->send_time = clock();
                network_send_packet(current->pack, addr);
            } else if (current->status != WAIT_SIGNAL) {
                double wait_time = WAIT_TIME_FOR_SEND;
                if (current->status == ERROR)
                    wait_time = 0;

                if (diff_time(current->send_time) >= wait_time) {
                    in_addr_t addr = current->addr->sin_addr.s_addr;

                    if (!network_send_packet(current->pack, addr)) {
                        perror("can't send packet");
                        return false;
                    }
                    current->send_time = clock();
                }
            }
        }
        if (!current)
            break;
        previous = current;
        current = current->next;
    }
    return true;
}

bool send_packet(NetworkContext *ctx, packet *pack, struct sockaddr_in *addr)
{
    addr->sin_addr.s_addr = htonl(addr->sin_addr.s_addr);
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

bool network_send_packet(packet *pack, in_addr_t addr)
{
    NetworkContext *ctx = network_init(PORT, addr);
    if (ctx == NULL) {
        perror("network error");
        return false;
    }
    bool result = send_packet(ctx, pack, &ctx->addr);
    network_fini(ctx);
    return result;
}

bool send_msg_to(char* buffer, unsigned buff_len, in_addr_t addr, uint8_t msg_id)
{
    if (buff_len == 0)
        return false;

    packet *packets = convert_to_packets(buffer, buff_len, MSG, msg_id);
    if (packets == NULL) {
        perror("can't convert packets");
        return false;
    }

    struct sockaddr_in sockaddr;
    sockaddr.sin_addr.s_addr = addr;

    if (packets[0].header.type == QUERY)
        for (unsigned i = 1; i < packets[0].header.num; ++i)
            if (!add_in_wait_queue(&packets[i], &sockaddr, WAIT_SIGNAL))
                return false;


    if (!add_in_wait_queue(&packets[0], &sockaddr, WAIT_SEND))
        return false;

    return true;
}

bool send_msg(char* buffer, unsigned buff_len, uint16_t id, uint8_t msg_id)
{
    in_addr_t *addr = find_addr(id);
    if (addr == NULL) {
        return false;
        //Возможно, здесь может быть что-то завязанное на регистрацию
    }
    bool status = send_msg_to(buffer, buff_len, *addr, msg_id);

    free(addr);
    return  status;
}

bool send_signal(in_addr_t addr, uint8_t msg_id)
{
    packet pack;
    pack.header = pack_header(0, SIGNAL, msg_id);

    bool status = network_send_packet(&pack, addr);
    return status;
}

void* start_send_manager()
{
    while(check_wait_queue()) { }

    return NULL;
}
