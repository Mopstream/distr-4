#ifndef __IFMO_DISTRIBUTED_CLASS_LAMPORT_H
#define __IFMO_DISTRIBUTED_CLASS_LAMPORT_H

#include "ipc.h"

timestamp_t get_lamport_time(void);
void inc_lamport_time(void);
void actualize_lamport_time(timestamp_t another_time);


#endif //__IFMO_DISTRIBUTED_CLASS_LAMPORT_H
