#include "../headers/receive_manager.h"


bool receive_pack(NetworkContext *ctx, packet_list* pack_list)
{

    unsigned len = sizeof(struct sockaddr_in);

    memcpy(&pack_list->addr, &ctx->addr, len);

    printf("!!!!!!!!!\n");
    ssize_t n = recvfrom(ctx->sock, pack_list->pack , PACKET_SIZE
                     , MSG_WAITALL, (struct sockaddr*) &pack_list->addr, &len);
    printf(":::::\n");
    if (n == -1) return false;

    return true;
}

void* start_receive_manager(NetworkContext *ctx)
{
    process_permission_flag = false;

    fifo_recv_last = fifo_recv_start;
    if (fifo_recv_last)
        while (fifo_recv_last->next != NULL)//FIXME переделать
            fifo_recv_last = fifo_recv_last->next;

    process_permission_flag = true;

    while(1) {
        packet_list *new = (packet_list*) malloc(sizeof(packet_list));
        if (!new) {
            perror("can't allocate memory");
            return;
        }

        if (!receive_pack(ctx, new)) {
            free(new);
            perror("can't receive packet");
            return;
        }

        new->id = UNKNOWN;
        new->next = NULL;

        if (fifo_recv_start) {

            fifo_recv_last->next = new;
            fifo_recv_last = fifo_recv_last->next;

        } else {

            fifo_recv_start = new;
            fifo_recv_last = fifo_recv_start;

        }
    }
}
