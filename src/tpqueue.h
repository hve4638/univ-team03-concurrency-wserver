#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int value;
    int priority;
} TPQueueNode;

typedef struct {
    TPQueueNode* nodes;
    int capacity;
    int size;
    pthread_mutex_t lock;
} TPQueue;

void tpqueue_init(TPQueue* queue, size_t capacity);
void tpqueue_push(TPQueue* queue, int value, int priority);
int tpqueue_pop(TPQueue*);
void tpqueue_free(TPQueue* queue);