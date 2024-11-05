#include "lamport.h"

timestamp_t time = 0;

timestamp_t get_lamport_time(void) {
    return time;
}
void inc_lamport_time(void) {
    time++;
}

void actualize_lamport_time(timestamp_t another_time) {
    time = (time > another_time ? time : another_time) + 1; 
}
