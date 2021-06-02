#include "../headers/network.h"

NetworkContext* network_init(int port, in_addr_t addr) {

    NetworkContext* ctx = (NetworkContext*) malloc(sizeof(NetworkContext));
    if (!ctx) goto error2;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0)  {
        perror("socket creation failed");
        goto error1;
    }
    ctx->sock = sock;

    memset(&ctx->addr, 0, sizeof(ctx->addr));

    ctx->addr.sin_family = AF_INET;
    ctx->addr.sin_port = htons(port);
    ctx->addr.sin_addr.s_addr = htonl(addr);


    return ctx;

    error1:
    free(ctx);
    error2:
    return NULL;
}

void network_fini(NetworkContext *ctx)
{
    close(ctx->sock);
    free(ctx);
}

in_addr_t s_addr_of(packet_list *pack_list)
{
    return pack_list->addr.sin_addr.s_addr;
}

packet_list* copy_of_packet_list(packet_list *pack_list)
{
    packet_list *copy = (packet_list *) malloc(sizeof(packet_list));
    if (copy == NULL)
        return NULL;

    copy->pack = copy_of_packet(pack_list->pack);
    if (copy->pack == NULL)
        return NULL;

    copy->id = pack_list->id;
    copy->addr = pack_list->addr;
    copy->next = NULL;

    return copy;
    error:
    free(copy);
    return NULL;
}