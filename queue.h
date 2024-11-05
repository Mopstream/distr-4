#ifndef __IFMO_DISTRIBUTED_CLASS_QUEUE_H
#define __IFMO_DISTRIBUTED_CLASS_QUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "ipc.h"

typedef struct node{
    local_id id;
    timestamp_t time;
    struct node* next;
} node_t;


node_t* create_node(timestamp_t time, local_id id);
void queue_push(node_t* node);
node_t* queue_pop(void);
node_t* queue_peek(void);
void print_queue(void);


#endif //__IFMO_DISTRIBUTED_CLASS_QUEUE_H
