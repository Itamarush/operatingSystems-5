#ifndef QUEUE_MOD_H
#define QUEUE_MOD_H

#include <pthread.h>

typedef struct NodeMod {
    void * payload;
    struct NodeMod* nextNode;
} NodeMod;

typedef struct QueueMod {
    NodeMod* head;
    NodeMod* tail;
    pthread_mutex_t mtx;
    pthread_cond_t cv;
} QueueMod;

QueueMod* allocateQueueMod();
void insertIntoQueueMod(QueueMod* qMod, void * payload);
void * removeFromQueueMod(QueueMod* qMod);
void deallocateQueueMod(QueueMod *qMod);

#endif
