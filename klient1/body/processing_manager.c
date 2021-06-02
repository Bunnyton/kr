#include "../headers/processing_manager.h"

bool make_new_mult_msg(packet_list *pack_list)
{
    mult_msg *new = (mult_msg*) malloc(sizeof(mult_msg));
    if (new == NULL)
        return false;

    new->packets = (packet_list**) malloc(sizeof(packet_list*) * pack_list->pack->header.num);
    if (new->packets == NULL)
        return false;

    new->packets[0] = copy_of_packet_list(pack_list);
    new->amount = pack_list->pack->header.num;
    new->next = NULL;

    for (unsigned i = 1; i < new->amount; ++i)
        new->packets[i] = NULL;

    if (start_mult_msg == NULL)
        start_mult_msg = new;
    else
        last_mult_msg->next = new;

    last_mult_msg = new;
    return true;
}

bool check_fill_mult_msg(mult_msg *multMsg)
{
    for(unsigned i = 0; i < multMsg->amount; ++i)
        if (multMsg->packets[i] == NULL)
            return false;
    return true;
}

void delete_pack_list(packet_list *pack_list)
{
    free(pack_list->pack);
    free(pack_list);
}

void delete_mult_msg(mult_msg *multMsg)
{
    mult_msg *current = start_mult_msg;
    if (multMsg == start_mult_msg) {
        start_mult_msg = NULL;
        current = NULL;
    }

    while(current != NULL && current->next != multMsg)
        current = current->next;

    if (current)
        current->next = multMsg->next;

    for(unsigned i = 0; i < multMsg->amount; ++i)
        delete_pack_list(multMsg->packets[i]);
    free(multMsg);
}

bool process_mult_msg(mult_msg *multMsg)
{
    unsigned packet_amount = multMsg->amount;
    char *last_msg = multMsg->packets[packet_amount - 1]->pack->msg;

    unsigned last_msg_len = strlen(last_msg);
    if (last_msg_len > PACKET_SYM_LOT) last_msg_len = PACKET_SYM_LOT;

    unsigned sym_amount = PACKET_SYM_LOT * (packet_amount - 1) + last_msg_len;

    char *buffer = (char*) malloc(sizeof(char) * sym_amount + 1);
    if (buffer == NULL) {
        perror("can't allocate memory");
        return false;
    }

    unsigned amount_sym_in_buff = 0;
    for (unsigned i = 0; i < packet_amount; ++i) {
        char *msg = multMsg->packets[i]->pack->msg;
        unsigned remain_amount = sym_amount - amount_sym_in_buff;

        unsigned min = MIN(PACKET_SYM_LOT, remain_amount);
        memcpy(buffer + amount_sym_in_buff, msg, min);

        amount_sym_in_buff += min;
    }
    buffer[amount_sym_in_buff] = '\0';

    if (!save_msg(buffer, multMsg->packets[0]->id)) {
        free(buffer);
        delete_mult_msg(multMsg);
        perror("can't save in file");
        return false;
    }

    free(buffer);
    delete_mult_msg(multMsg);
    return true;
}

void add_in_mult_msg(packet_list *pack_list)
{
    mult_msg *current = start_mult_msg;
    while (current != NULL) {

        if (s_addr_of(current->packets[0]) == s_addr_of(pack_list))
        {
            unsigned num = pack_list->pack->header.num;
            current->packets[num] = copy_of_packet_list(pack_list);
            if (check_fill_mult_msg(current))
                process_mult_msg(current);
            break;
        }
            current = current->next;
    }
}

bool add_user_rec(uint16_t id, char *nick, in_addr_t addr)
{
    char *id_str = uint16_t_to_str(id);
    if (id_str == NULL)
        return false;

    struct in_addr saddr;
    saddr.s_addr = htonl(addr);

    char *ip = inet_ntoa(saddr);
    if (ip == NULL)
        goto error2;

    unsigned long len_id_str = strlen(id_str);
    unsigned long len_nick = strlen(nick);
    unsigned long len_ip = strlen(ip);

    char *record = (char *) malloc(sizeof(char) * (len_id_str + len_nick + len_ip + 4));
    if (record == NULL)
        goto error1;

    memcpy(record, id_str, len_id_str);
    record[len_id_str] = ' ';

    memcpy(record + len_id_str + 1, nick, len_nick);
    record[len_id_str + len_nick + 1] = ' ';

    memcpy(record + len_id_str + len_nick + 2, ip, len_ip);
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
            send_signal(s_addr_of(pack_list), pack_list->pack->header.msg_id);
            break;

        case QUERY:

            make_new_mult_msg(pack_list);
            send_signal(s_addr_of(pack_list), pack_list->pack->header.msg_id);
            break;

        case MULT_MSG:

            send_signal(s_addr_of(pack_list), pack_list->pack->header.msg_id);
            add_in_mult_msg(pack_list);
            break;

        case SIGNAL:
        {
            unsigned msg_id = pack_list->pack->header.msg_id;
            wait_packet_list *helper = find_in_wait_queue(s_addr_of(pack_list), WAIT_DELIVER, msg_id);
            if (helper != NULL) {
                if (helper->pack->header.type == QUERY) {
                    wait_packet_list* current_wait_pack;
                    current_wait_pack = find_in_wait_queue(s_addr_of(pack_list), WAIT_SIGNAL, msg_id);

                    while (current_wait_pack != NULL) {
                        current_wait_pack->status = WAIT_SEND;
                        current_wait_pack = find_in_wait_queue(s_addr_of(pack_list), WAIT_SIGNAL, msg_id);
                    }
                }

                if (helper->status != DELIVERED)
                    helper->status = DELIVERED;

            }
            break;
        }
        case SIGN:
        {
            uint16_t new_id = make_id();
            if (new_id == UNKNOWN) {
                //выход из системы
            }
            add_user_rec(make_id(), pack_list->pack->msg, pack_list->addr.sin_addr.s_addr);
            char *my_nick = find_nick(MY_ID);
            if (my_nick == NULL) {
                //выход из системы
            }

            unsigned msg_id = pack_list->pack->header.msg_id;
            if (!send_msg_to(my_nick, strlen(my_nick), pack_list->addr.sin_addr.s_addr, msg_id)) {
                //выход из системы
            }

        }
        default:
            break;
    }
}


void* start_processing_manager()
{
    while(!fifo_recv_start) { }

    process(fifo_recv_start);
    while(true) {
        if(process_permission_flag) {
            if (fifo_recv_start->next != NULL) {
                packet_list *helper = fifo_recv_start->next;
                delete_pack_list(fifo_recv_start);
                fifo_recv_start = helper;
                process(fifo_recv_start);
            }
        }
    }
}
