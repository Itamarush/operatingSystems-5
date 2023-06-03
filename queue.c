#include <stdlib.h>
#include "queue.h"

QueueMod* allocateQueueMod() {
    QueueMod* qMod = malloc(sizeof(QueueMod));
    qMod->head = qMod->tail = NULL;
    pthread_mutex_init(&qMod->mtx, NULL);
    pthread_cond_init(&qMod->cv, NULL);
    return qMod;
}

void insertIntoQueueMod(QueueMod* qMod, void* payload) {
    pthread_mutex_lock(&qMod->mtx);
    
    NodeMod* newModNode = malloc(sizeof(NodeMod));
    newModNode->payload = payload;
    newModNode->nextNode = NULL;

    if(qMod->tail == NULL) {
        qMod->head = qMod->tail = newModNode;
        pthread_cond_signal(&qMod->cv);
    } else {
        qMod->tail->nextNode = newModNode;
        qMod->tail = newModNode;
    }

    pthread_mutex_unlock(&qMod->mtx);
}

void* removeFromQueueMod(QueueMod* qMod) {
    pthread_mutex_lock(&qMod->mtx);
    
    while(qMod->head == NULL) {
        pthread_cond_wait(&qMod->cv, &qMod->mtx);
    }

    NodeMod* tempModNode = qMod->head;
    void * item = tempModNode->payload;

    qMod->head = qMod->head->nextNode;

    if(qMod->head == NULL) {
        qMod->tail = NULL;
    }

    free(tempModNode);

    pthread_mutex_unlock(&qMod->mtx);
    
    return item;
}

void deallocateQueueMod(QueueMod *qMod)
{
    pthread_mutex_lock(&qMod->mtx);
    NodeMod *tempModNode;
    while (qMod->head != NULL)
    {
        tempModNode = qMod->head;
        qMod->head = tempModNode->nextNode;
        free(tempModNode->payload);
        free(tempModNode);
    }
    pthread_cond_destroy(&qMod->cv);
    pthread_mutex_destroy(&qMod->mtx);
    pthread_mutex_unlock(&qMod->mtx);
    free(qMod);
}
