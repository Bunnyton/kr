#include <pthread.h>
#include "../headers/system.h"

bool sign_in()
{
    NetworkContext *ctx = network_init(PORT, INADDR_BROADCAST);
    if (ctx == NULL)
        return false;


    int broadcast_enable = 1;
    if (setsockopt(ctx->sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1){
        perror("sockoption error");
        goto error1;
    }

    FILE *file = fopen(USER_LIST_FILE, "r");
    if (file == NULL) {
        perror("file error");
        goto error1;
    }

    char *helper = malloc(sizeof(char) * MAXLINE);
    if (helper == NULL) {
        perror("can't allocate memory");
        goto error2;
    }

    for (int i = 0; i < 2; ++i)
        if (read_to(file, ' ', helper) == EOF) {//Пропуск id(1) 1 id - свой id по умолчанию у всех
            perror("file error");//На второй раз уже будет считано имя текущего пользователя
            goto error2;
        }

    packet *pack = convert_to_packets(helper, sizeof(helper), UNKNOWN, SIGN);
    if (pack == NULL){
        perror("convert error");
        goto error3;
    }

    for(int i = 0; i < 10; ++i)
        if (!send_packet(ctx, &pack[0], &ctx->addr)) {
            perror("send error");
            goto error4;
        }

    network_fini(ctx);
    return true;

    error4:
    free(pack);

    error3:
    free(helper);

    error2:
    fclose(file);

    error1:
    network_fini(ctx);
    return false;
}

bool sign_up()
{
    char *nick = malloc(sizeof(char) * MAXLINE);
    if (nick == NULL) {
        perror("can't allocate memory");
        return false;
    }

    do {
        printf("Enter your nickname: ");
        if (read_to(stdin,'\n', nick) == EOF){
            perror("file error");
            return false;
        }
    } while(strlen(nick) > PACKET_SYM_LOT);

    add_user_rec(1, nick, INADDR_LOOPBACK);

    return true;
}

NetworkContext* system_init(){
    NetworkContext *ctx = network_init(PORT, INADDR_ANY);
    if (!ctx) return NULL;

    int broadcast_enable = 1;

    /*if (setsockopt(ctx->sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1){
        perror("setsockopt failed");
        goto error;
    }*/

    if (bind(ctx->sock, (const struct sockaddr *) &ctx->addr, sizeof(ctx->addr)) < 0){
        perror("can't bind socket");
        goto error;
    }

    fifo_recv_start = NULL;
    fifo_recv_last = NULL;
    process_permission_flag = false;
    sign_up_flag = false;

    FILE *file = fopen(USER_LIST_FILE, "r");
    if (file == NULL) {
        if (!sign_up()) {
            goto error;
        }
    } else
        fclose(file);

    return ctx;
    error:
    network_fini(ctx);
    return NULL;
}

void system_start(NetworkContext *ctx)
{
    pthread_t id1, id2, id3;
    pthread_create(&id1, NULL, (void*(*)(void *))start_receive_manager, ctx);
    pthread_create(&id2, NULL, (void*(*)(void *)) start_processing_manager, NULL);
    //pthread_create(&id3, NULL, start_send, NULL);

    sign_in();
    packet_list *helper = NULL;

    char *buffer = malloc(sizeof(char) * MAXLINE);
    while(1) {
        printf("Message to id 2: ");
        read_to(stdin, '\n', buffer);
        send_msg(buffer, strlen(buffer), 2);
        if (fifo_recv_start != NULL) {
            if (helper == NULL) {
                helper = fifo_recv_start;
                printf("Message:%s", helper->pack->msg);
            }
            while (helper->next != NULL) {
                printf("Message from %d: %s\n", helper->next->pack->header.send_id, helper->next->pack->msg);
                helper = helper->next;
            }
        }
    }
}