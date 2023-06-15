#include "tpqueue.h"


static void swap_node(TPQueueNode* node1, TPQueueNode* node2) {
    TPQueueNode nodet = *node1;
    *node1 = *node2;
    *node2 = nodet;
}

void tpqueue_init(TPQueue* queue, size_t capacity) {
    size_t node_capacity = capacity * sizeof(TPQueueNode);
    queue->nodes = (TPQueueNode*)malloc(node_capacity);
    queue->capacity = capacity;
    queue->size = 1;
    pthread_mutex_init(&(queue->lock), NULL);
}

void tpqueue_free(TPQueue* queue) {
    free(queue->nodes);
    pthread_mutex_destroy(&(queue->lock));
}

void tpqueue_push(TPQueue* queue, int value, int priority) {
    pthread_mutex_lock(&queue->lock);
    int index = (queue->size++);
    TPQueueNode *node = &queue->nodes[index];
    TPQueueNode *pnode;
    node->priority = priority;
    node->value = value;

    while(1 < index) {
        pnode = &queue->nodes[index/2];
        if (pnode->priority > node->priority) {
            swap_node(pnode, node);
        }
        node = pnode;

        index /= 2;
    }
    pthread_mutex_unlock(&queue->lock);
}

int tpqueue_pop(TPQueue* queue) {
    int result;

    //#if (queue->size <= 1) return -1;

    pthread_mutex_lock(&(queue->lock));

    if (queue->size > 1) {
        int size = (--queue->size);
    
        result = queue->nodes[1].value;
        queue->nodes[1] = queue->nodes[size];

        int index = 1;
        TPQueueNode* current;
        TPQueueNode* left;
        TPQueueNode* right;
        while (index*2 < size) {
            current = &queue->nodes[1];
            left = &queue->nodes[index*2];
            right = &queue->nodes[index*2+1];

            int lp, rp, cp;
            cp = current->priority;
            lp = left->priority;
            rp = right->priority;
            if (cp <= lp && cp <= rp) {
                break;
            }
            else if (lp < rp) {
                swap_node(current, left);
                index = index*2;
            }
            else {
                swap_node(current, right);
                index = index*2 + 1;
            }
        }
    }
    else {
        result = -1;
    }

    pthread_mutex_unlock(&(queue->lock));
    return result;
}
