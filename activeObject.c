#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "activeObject.h"
#include <stdint.h>

void* executeInitialTask(void* params) {
    ActiveObj* currentObj = (ActiveObj*) params;
    void* taskData = removeFromQueueMod(currentObj->taskQueue);
    currentObj->currentTask(taskData);
    return NULL;
}

void* executeRegularTask(void* params) {
    ActiveObj* currentObj = (ActiveObj*) params;
    int taskData = (int)(uintptr_t)removeFromQueueMod(currentObj->taskQueue);
    if (taskData == -3) {
        currentObj->isActive = 1;
    } else {
        currentObj->currentTask(&taskData);
    }
    if (currentObj->nextObj) {
        insertIntoQueueMod(currentObj->nextObj->taskQueue, (void *)(uintptr_t)taskData);
    }
    return NULL;
}

void* execute(void* params) {
    ActiveObj* currentObj = (ActiveObj*) params;
    while (!currentObj->isActive) {
        if (currentObj->isInitial) {
            executeInitialTask(currentObj);
        } else {
            executeRegularTask(currentObj);
        }
    }
    return NULL;
}


ActiveObj* allocateMemory(TaskPtr taskFunc, ActiveObj* nextObj) {
    ActiveObj* currentObj = (ActiveObj*) malloc(sizeof(ActiveObj));
    if (!currentObj) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    currentObj->taskQueue = allocateQueueMod();
    if (!currentObj->taskQueue) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    currentObj->thread = (pthread_t)calloc(1, sizeof(pthread_t));
    if (!currentObj->thread) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    return currentObj;
}

ActiveObj* createActiveObject(TaskPtr taskFunc, ActiveObj* nextObj) {
    ActiveObj* currentObj = allocateMemory(taskFunc, nextObj);

    currentObj->isActive = 0;
    currentObj->isInitial = 0;
    currentObj->nextObj = nextObj;
    currentObj->currentTask = taskFunc;

    if (pthread_create(&(currentObj->thread), NULL, execute, currentObj)) {
        perror("pthread creation failed");
        exit(EXIT_FAILURE);
    }

    return currentObj;
}


QueueMod* fetchQueue(ActiveObj* obj) {
    return obj->taskQueue;
}

void stopActiveObject(ActiveObj* obj) {
    obj->isActive = 1;
    pthread_cancel(obj->thread);
    pthread_join(obj->thread, NULL);
    deallocateQueueMod(obj->taskQueue);
    free(obj);
}
