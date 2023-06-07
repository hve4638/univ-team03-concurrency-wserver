#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 우선순위 큐 노드 정의
typedef struct {
    int priority;
} Node;

typedef struct {
    Node* nodes;
    int capacity;
    int size;
    pthread_mutex_t mutex;
} PriorityQueue;

void PriorityQueue_Init(PriorityQueue* queue, int capacity) {
    queue->nodes = (Node*)malloc(capacity * sizeof(Node));
    queue->capacity = capacity;
    queue->size = 0;
    pthread_mutex_init(&(queue->mutex), NULL);
}

void PriorityQueue_Destroy(PriorityQueue* queue) {
    pthread_mutex_destroy(&(queue->mutex));
    free(queue->nodes);
}

void PriorityQueue_Push(PriorityQueue* queue, int priority) {
    pthread_mutex_lock(&(queue->mutex));

    // 큐가 가득 찼을 경우 크기를 확장
    if (queue->size >= queue->capacity) {
        int newCapacity = queue->capacity * 2;
        queue->nodes = (Node*)realloc(queue->nodes, newCapacity * sizeof(Node));
        queue->capacity = newCapacity;
    }

    // 새 노드 생성 및 우선순위 설정
    Node newNode;
    newNode.priority = priority;

    // 새 노드를 적절한 위치에 삽입
    int index = queue->size;
    while (index > 0 && queue->nodes[index - 1].priority > priority) {
        queue->nodes[index] = queue->nodes[index - 1];
        index--;
    }
    queue->nodes[index] = newNode;
    queue->size++;

    pthread_mutex_unlock(&(queue->mutex));
}

int PriorityQueue_Pop(PriorityQueue* queue) {
    pthread_mutex_lock(&(queue->mutex));

    if (queue->size <= 0) {
        pthread_mutex_unlock(&(queue->mutex));
        return -1; // 큐가 비어있음
    }

    Node node = queue->nodes[0];
    int priority = node.priority;

    for (int i = 1; i < queue->size; i++) {
        queue->nodes[i - 1] = queue->nodes[i];
    }
    queue->size--;

    pthread_mutex_unlock(&(queue->mutex));

    return priority;
}