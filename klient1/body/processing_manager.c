#include "../headers/processing_manager.h"

bool add_user_rec(uint16_t id, char *nick, in_addr_t addr)
{
    char *id_str = uint16_t_to_str(id);
    if (id_str == NULL)
        return false;

    struct in_addr saddr;
    saddr.s_addr = addr;

    char *ip = inet_ntoa(saddr);
    if (ip == NULL)
        goto error2;

    unsigned long len_id_str = strlen(id_str);
    unsigned long len_nick = strlen(nick);
    unsigned long len_ip = strlen(ip);

    char *record = (char *) malloc(sizeof(char) * (len_id_str + len_nick + len_ip + 4));
    if (record == NULL)
        goto error1;

    stradd(record, id_str, 0);
    record[len_id_str] = ' ';

    stradd(record, nick, len_id_str + 1);
    record[len_id_str + len_nick + 1] = ' ';

    stradd(record, ip, len_id_str + len_nick + 2);
    record[len_id_str + len_nick + len_ip + 2] = '\n';

    record[len_id_str + len_nick + len_ip + 3] = '\0';

    save_in(record, USER_LIST_FILE);

    return true;

    error1:
    free(ip);

    error2:
    free(id_str);
    return false;
}

char* form_mult_msg(packet_list *pack_list)
{
    if (pack_list->pack->header.type != QUERY)
        return NULL;

    uint32_t id = pack_list->pack->header.send_id;
    uint32_t amount = pack_list->pack->header.num;

    char *buffer = (char *) malloc(sizeof(char) * PACKET_SYM_LOT * amount);
    if (buffer == NULL) {
        perror("can't allocate memory");
        return NULL;
    }

    stradd(buffer, pack_list->pack->msg, 0);

    pack_list = fifo_recv_last ;
    //send_signal()

    uint32_t i = 1;
    while(i < amount){
        if (pack_list->next){
            pack_list = pack_list->next;

            if (pack_list->pack->header.send_id == id &&
                pack_list->pack->header.type == MULT_MSG) {

                stradd(buffer, pack_list->pack->msg,
                       pack_list->pack->header.num * PACKET_SYM_LOT);
                ++i;
            }

        }
    }
    return buffer;
}

void process_mult_msg(packet_list *pack_list)
{
    process_permission_flag = false;
    char *buffer = form_mult_msg(pack_list);
    if (!save_msg(buffer, pack_list->pack->header.send_id)){
        //Выход из системы
    }
}

void process(packet_list *pack_list)
{
    pack_list->id = find_id(pack_list->addr.sin_addr.s_addr);

    if (pack_list->id != UNKNOWN || (pack_list->id == UNKNOWN && pack_list->pack->header.type == SIGN))
    switch(pack_list->pack->header.type)
    {
        case MSG:
            if (!save_msg(pack_list->pack->msg, pack_list->id)){
                //Выход из системы
            }
            break;
        case QUERY:
            process_mult_msg(pack_list);//FIXME Запустить в параллель
            break;
        case SIGN: {
            uint16_t new_id = make_id();
            if (new_id == UNKNOWN) {
                //выход из системы
            }
            add_user_rec(make_id(), pack_list->pack->msg, pack_list->addr.sin_addr.s_addr);
            char *my_nick = find_nick(MY_ID);
            if (my_nick == NULL) {
                //выход из системы
            }

            if (!send_msg_to(my_nick, strlen(my_nick), pack_list->addr.sin_addr.s_addr)) {
                //выход из системы
            }

        }
        default:
            break;
    }
}

void start_processing_manager()
{
    while(!fifo_recv_start) {} //FIXME добавить засыпание

    process(fifo_recv_start);
    while(true) {
        if(process_permission_flag) {
            if (fifo_recv_start->next != NULL) {
                packet_list *helper = fifo_recv_start->next;
                free(fifo_recv_start);
                fifo_recv_start = helper;
                process(fifo_recv_start);
            }
        }
    }
}
