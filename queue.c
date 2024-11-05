#include "queue.h"

node_t* queue = NULL;

node_t* create_node(timestamp_t time, local_id id) {
    node_t* new = malloc(sizeof(node_t));
    new->id = id;
    new->time = time;
    new->next = NULL;
    return new;
}
void queue_push(node_t* node) {
    node_t* prev = NULL;
    node_t* this = queue;
    while(this && (this->time < node->time || (this->time == node->time && this->id < node->id))) {
        prev = this;
        this = this->next;
    }
    if (prev) {
        prev->next = node;
        node->next = this;
    } else {
        node->next = this;
        queue = node;
    }
    return;
}

node_t* queue_pop(void) {
    node_t* head = queue;
    if (!queue) {
        fprintf(stderr, "ERROR pop from empty queue\n");
        return NULL;
    }
    queue = queue->next;
    return head;
}

node_t* queue_peek(void) {
    node_t* head = queue;
    if (!queue) {
        fprintf(stderr, "ERROR peek from empty queue\n");
        return NULL;
    }
    return head;
}

void print_queue(void) {
    node_t* this = queue;
    fprintf(stderr, "QUEUE: ");
    while(this) {
        fprintf(stderr, "{%d; %d} -> ", this->time, this->id);
        this = this->next;
    }
    fprintf(stderr, "NULL\n");
}
