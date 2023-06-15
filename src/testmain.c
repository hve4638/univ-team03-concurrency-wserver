#include "tpqueue.h"

void tpqueue_init(TPQueue* queue, size_t capacity);
void tpqueue_push(TPQueue* queue, int value, int priority);
int tpqueue_pop(TPQueue*);
void tpqueue_free(TPQueue* queue);

int main() {
    TPQueue queue;

    tpqueue_init(&queue, 64);
    tpqueue_push(&queue, 1, 50);
    tpqueue_push(&queue, 2, 10);
    tpqueue_push(&queue, 3, 25);
    tpqueue_push(&queue, 4, 25);

    printf("> %d\n", tpqueue_pop(&queue));
    printf("> %d\n", tpqueue_pop(&queue));
    printf("> %d\n", tpqueue_pop(&queue));
    printf("> %d\n", tpqueue_pop(&queue));
    tpqueue_free(&queue);
}