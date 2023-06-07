#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define QUEUE_SIZE 100 // 임시

typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
    pthread_mutex_t lock;
} ThreadSafeQueue;

void init_queue(ThreadSafeQueue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
    pthread_mutex_init(&queue->lock, NULL);
}


void enqueue(ThreadSafeQueue* queue, int data) {

    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->data = data;
    newNode->next = NULL;

    pthread_mutex_lock(&queue->lock); // 크리티컬 섹션 시작

    // 큐가 비어있을 경우
    if (queue->rear == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
    }
    // 큐가 비어있지 않을 경우
    else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    pthread_mutex_unlock(&queue->lock);
}

int dequeue(ThreadSafeQueue* queue) {
    pthread_mutex_lock(&queue->lock);

    // 큐가 비어있을 경우
    if (queue->front == NULL) {
        pthread_mutex_unlock(&queue->lock);
        return -1;
    }

    QueueNode* removedNode = queue->front;
    int data = removedNode->data;
    queue->front = queue->front->next;
    free(removedNode);

    // 큐가 비어있을 경우 rear도 NULL로 설정
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    pthread_mutex_unlock(&queue->lock);

    return data;
}

void destroy_queue(ThreadSafeQueue* queue) {
}

void* producer(void* arg) {
}
