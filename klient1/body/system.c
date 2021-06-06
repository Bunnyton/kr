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

    packet *pack = convert_to_packets(helper, sizeof(helper), SIGN, 0);//msg id ни на что не влияет
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

    if (setsockopt(ctx->sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1){
        perror("setsockopt failed");
        goto error;
    }

    if (bind(ctx->sock, (const struct sockaddr *) &ctx->addr, sizeof(ctx->addr)) < 0){
        perror("can't bind socket");
        goto error;
    }

    fifo_recv_start = NULL;
    fifo_recv_last = NULL;

    wait_pack_start = NULL;
    wait_pack_last = NULL;

    users_last_msg_id.msg_id = NULL;
    users_last_msg_id.size = 0;
    current_msg_id = 1;

    receive_stop_flag = true;
    wait_queue_stop_flag = true;
    exit_flag = false;

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
    pthread_create(&id3, NULL, start_send_manager, NULL);

    sign_in();

    char *buffer = malloc(sizeof(char) * MAXLINE);
    if (buffer == NULL) exit_flag = true;

    while(true && !exit_flag) {
        unsigned id  = 0;
        printf("Id of receive user (0 = exit): ");
        scanf("%d", &id);

        if (id == 0) {
            exit_flag = true;
            sleep(2);
            break;
        }

        printf("Message: ");
        read_to(stdin, '\n', buffer);

        send_msg(buffer, strlen(buffer), id, current_msg_id);
        ++current_msg_id;
    }
}