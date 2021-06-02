#pragma  once

#include "network.h"
#include "filework.h"
#include "str_addit_methods.h"
#include "time.h"

#define WAIT_TIME_FOR_SEND 1
#define WAIT_TIME_FOR_SIGNAL 5

enum _deliver_status {
      WAIT_SIGNAL
    , WAIT_DELIVER
    , DELIVERED
    , ERROR
    , WAIT_SEND
};
typedef enum _deliver_status deliver_status;

struct _wait_packet_list{
    packet *pack;
    struct sockaddr_in *addr;
    uint16_t id;
    deliver_status status;
    time_t send_time;
    struct _wait_packet_list *next;
};
typedef struct _wait_packet_list wait_packet_list;


wait_packet_list *start;
wait_packet_list *last;

unsigned current_msg_id;

char* read_from_keyboard();

bool send_packet(NetworkContext *ctx, packet *pack, struct sockaddr_in *addr);
bool network_send_packet(packet *pack, in_addr_t addr);
bool send_msg_to(char *buffer, unsigned buffer_len, in_addr_t addr, unsigned msg_id);
bool send_msg(char *buffer, unsigned buffer_len, uint16_t id, unsigned msg_id);
bool send_signal(in_addr_t addr, unsigned msg_id);

bool add_in_wait_queue(packet *pack, struct sockaddr_in *addr, deliver_status status);
wait_packet_list* find_in_wait_queue(in_addr_t addr, deliver_status status, unsigned msg_id);
bool check_wait_queue();

void* start_send_manager();
