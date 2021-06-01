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

bool add_in_queue(packet *pack, struct sockaddr_in *addr);

wait_packet_list *start;
wait_packet_list *last;

char* read_from_keyboard();

bool send_packet(NetworkContext *ctx, packet *pack, struct sockaddr_in *addr);
bool send_msg_to(char *buffer, unsigned buffer_len, in_addr_t addr);
bool send_msg(char *buffer, unsigned buffer_len, uint16_t id);
