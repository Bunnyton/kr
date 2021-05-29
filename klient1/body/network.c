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



