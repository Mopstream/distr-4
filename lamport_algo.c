#include "pa2345.h"
#include "util.h"
#include "lamport.h"
#include "queue.h"


int request_cs(const void * self) {
    local_id this = (local_id)(uint64_t) self;
    Message *msg = malloc(sizeof(Message));
    inc_lamport_time();
    node_t* req = create_node(get_lamport_time(), this);
    queue_push(req);
    MessageHeader head = {
        .s_local_time = get_lamport_time(),
        .s_magic = MESSAGE_MAGIC,
        .s_payload_len = 0,
        .s_type = CS_REQUEST
    };
    msg->s_header = head;

    for (local_id i = 1; i <= n; ++i){
        if(i != this) {
            send((void *)self, i, msg);
        }
    }
    uint32_t done = 0;
    uint32_t reply_cnt = 0;
    while (reply_cnt < n - 1 || queue_peek()->id != this) {
        local_id from = receive_any((void *)self, msg);
        actualize_lamport_time(msg->s_header.s_local_time);
        switch(msg->s_header.s_type) {
            case CS_REQUEST:
                queue_push(create_node(msg->s_header.s_local_time, from));
                inc_lamport_time();
                MessageHeader head = {
                    .s_local_time = get_lamport_time(),
                    .s_magic = MESSAGE_MAGIC,
                    .s_payload_len = 0,
                    .s_type = CS_REPLY
                };
                msg->s_header = head;
                send((void*) self, from, msg);
                break;
            case CS_REPLY:
                ++reply_cnt;
                break;
            case CS_RELEASE:
                free(queue_pop());
                break;
            case DONE:
                ++done;
                break;
            default:
                fprintf(stderr, "Unknown mes from %d with type %d\n", from, msg->s_header.s_type);
                break;
        }
    }
    return done;
}

int release_cs(const void * self) {
    local_id this = (local_id)(uint64_t) self;
    Message *msg = malloc(sizeof(Message));
    inc_lamport_time();
    MessageHeader head = {
        .s_local_time = get_lamport_time(),
        .s_magic = MESSAGE_MAGIC,
        .s_payload_len = 0,
        .s_type = CS_RELEASE
    };
    msg->s_header = head;

    for (local_id i = 1; i <= n; ++i){
        if(i != this) {
            send((void *)self, i, msg);
        }
    } 
    free(queue_pop());
    free(msg);
    return 0;
}
