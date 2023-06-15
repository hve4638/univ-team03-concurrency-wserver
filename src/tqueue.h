#ifndef __TQUEUE__
#define __TQUEUE__
#include <pthread.h>

typedef struct TQueue {
    void* first;
    void* last;
    pthread_mutex_t* lock;
} TQueue;

int tqueue_init(TQueue*);
int tqueue_destroy(TQueue*);
int tqueue_push(TQueue*, int);
int tqueue_pop(TQueue*);
#endif