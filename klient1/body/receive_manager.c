#include "../headers/receive_manager.h"


bool receive_pack(NetworkContext *ctx, packet_list* pack_list)
{

    unsigned len = sizeof(struct sockaddr_in);

    memcpy(&pack_list->addr, &ctx->addr, len);

    ssize_t n = recvfrom(ctx->sock, pack_list->pack , PACKET_SIZE
                     , MSG_WAITALL, (struct sockaddr*) &pack_list->addr, &len);
    if (n == -1) return false;

    return true;
}

void* start_receive_manager(NetworkContext *ctx)
{
    while(true && !exit_flag) {
        packet_list *new = (packet_list*) malloc(sizeof(packet_list));
        if (!new) {
            perror("can't allocate memory");
            goto error;
        }

        new->pack = (packet*) malloc(sizeof(packet));
        if (!new->pack) {
            free(new);
            perror("can't allocate memory");
            goto error;
        }

        if (!receive_pack(ctx, new)) {
            free(new->pack);
            free(new);
            perror("can't receive packet");
            goto error;
        }

        new->id = UNKNOWN;
        new->next = NULL;

        while (!receive_stop_flag) { }
        if (fifo_recv_start) {

            fifo_recv_last->next = new;
            fifo_recv_last = fifo_recv_last->next;

        } else {

            fifo_recv_start = new;
            fifo_recv_last = fifo_recv_start;
        }
    }
    error:
    exit_flag = true;
    return NULL;
}
