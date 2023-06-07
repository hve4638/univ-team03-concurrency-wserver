#include <pthread.h>

typedef struct tqueue {
    void* first;
    void* last;
    pthread_mutex_t* lock;
} tqueue;

int tqueue_init(tqueue*);
int tqueue_destroy(tqueue*);
int tqueue_push(tqueue*, int);
int tqueue_pop(tqueue*);