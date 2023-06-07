
#include "tqueue.h"

typedef struct tqueue_node {
    int tid;
    struct tqueue_node* next;
} tqueue_node;

int tqueue_init(tqueue* queue) {
    queue->first = 0;
    queue->last = 0;
    queue->lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(queue->lock, NULL);
}

int tqueue_destroy(tqueue* queue) {
    while(tqueue_pop(queue) != -1) {
        
    }
    pthread_mutex_destroy(queue->lock);
    queue->lock = 0;
}

int tqueue_push(tqueue* queue, int tid) {
    pthread_mutex_lock(queue->lock);
    tqueue_node* node = (tqueue_node*)malloc(sizeof(tqueue_node));
    node->tid = tid;
    node->next = 0;

    if (queue->first == 0) {
        queue->first = node;
    }
    queue->last = node;

    pthread_mutex_unlock(queue->lock);
}

int tqueue_pop(tqueue* queue) {
    int result;
    tqueue_node* node;
    tqueue_node* next;
    pthread_mutex_lock(queue->lock);

    node = queue->first;
    if (node == 0) {
        result = -1;
    }
    else {
        result = node->tid;

        next = node->next;
        if (next == 0) {
            queue->first = 0;
            queue->last = 0;
        }
        else {
            queue->first = next;
        }
        free(node);
    }
    pthread_mutex_unlock(queue->lock);
    return result;
}
