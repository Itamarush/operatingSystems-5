#ifndef ACTIVEOBJECT_H
#define ACTIVEOBJECT_H

#include <pthread.h>
#include "queue.h"

typedef void (*TaskPtr)(void*);

typedef struct ActiveObj {
    QueueMod* taskQueue;
    int isActive;
    int isInitial;
    struct ActiveObj* nextObj;
    TaskPtr currentTask;
    pthread_t thread;
} ActiveObj;

void* executeInitialTask(void*);
void* executeRegularTask(void*);
void* execute(void*);
ActiveObj* allocateMemory(TaskPtr, ActiveObj*);
ActiveObj* createActiveObject(TaskPtr, ActiveObj*);
QueueMod* fetchQueue(ActiveObj*);
void stopActiveObject(ActiveObj*);

#endif
