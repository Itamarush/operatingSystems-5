#include <unistd.h>
#include "activeObject.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int check_prime(unsigned int num) {
    for (int i = 2; i < num / 2; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}


void generateRandomNumbers(int N, ActiveObj *obj1, int seed) {
    srand(seed);
    for (size_t i = 0; i < N; i++) {
        int number = (rand() % 900000) + 100000;
        insertIntoQueueMod(obj1->nextObj->taskQueue, (void *)(uintptr_t)number);
        usleep(1000);
    }
}

void firstFunc(void *argv) {
    void **data_initial = (void **)argv;
    int N = (int)(uintptr_t)data_initial[0];
    int seed = (int)(uintptr_t)data_initial[1];
    ActiveObj *obj1 = (ActiveObj *)data_initial[2];

    generateRandomNumbers(N, obj1, seed);

    insertIntoQueueMod(obj1->nextObj->taskQueue, (void *)-3);
    obj1->isActive = 1;
}


void printNumberAndCheckPrime(int *number) {
    printf("%d\n%s\n", *number, check_prime(*number) ? "true" : "false");
}

void modifyNumberByAdding11(int *number) {
    *number += 11;
}

void modifyNumberBySubtracting13(int *number) {
    *number -= 13;
}

void modifyNumberByAdding2AndPrint(int *number) {
    printf("%d\n", *number);
    *number += 2;
    printf("%d\n", *number);
}

void secondFunc(void *data) {
    int *number = (int *)data;
    printNumberAndCheckPrime(number);
    modifyNumberByAdding11(number);
}

void thirdFunc(void *data) {
    int *number = (int *)data;
    printNumberAndCheckPrime(number);
    modifyNumberBySubtracting13(number);
}

void fourthFunc(void *data) {
    int *number = (int *)data;
    printNumberAndCheckPrime(number);
    modifyNumberByAdding2AndPrint(number);
}

void initiateActiveObj(ActiveObj *actObj, int N, int seed){
    void **initial_data = (void **)malloc(sizeof(void *) * 3);
    if (initial_data == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }

    initial_data[0] = (void *)(uintptr_t)N;
    initial_data[1] = (void *)(uintptr_t)seed;
    initial_data[2] = actObj;

    actObj->isInitial = 1;

    insertIntoQueueMod(actObj->taskQueue, (void *)initial_data);
}


typedef void (*activeObjectFunc)(void *);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s N [seed]\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int seed = argc > 2 ? atoi(argv[2]) : time(NULL);

    activeObjectFunc funcArray[] = {firstFunc, secondFunc, thirdFunc, fourthFunc};
    int numObjects = sizeof(funcArray) / sizeof(activeObjectFunc);
    ActiveObj *objArray[numObjects];

    // Create ActiveObjects in reverse order, so that nextObj can be correctly assigned
    for (int i = numObjects - 1; i >= 0; --i) {
        ActiveObj *nextObj = (i == numObjects - 1) ? NULL : objArray[i + 1];
        objArray[i] = createActiveObject(funcArray[i], nextObj);
    }

    void **data_initial = (void **)malloc(sizeof(void *) * 3);
    if (data_initial == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    data_initial[0] = (void *)(uintptr_t)N;
    data_initial[1] = (void *)(uintptr_t)seed;
    data_initial[2] = objArray[0];

    objArray[0]->isInitial = 1;

    insertIntoQueueMod(objArray[0]->taskQueue, (void *)data_initial);

    sleep(1);

    // Stop ActiveObjects
    for (int i = 0; i < numObjects; ++i) {
        stopActiveObject(objArray[i]);
    }

    free(data_initial);

    return 0;
}
