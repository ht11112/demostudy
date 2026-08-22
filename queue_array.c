/* queue_array.c - circular queue with size counter */
#include <stdio.h>
#include <stdbool.h>

#define MAX_SIZE 5

typedef struct {
    int data[MAX_SIZE];
    int front;   /* dequeue position */
    int rear;    /* next enqueue position */
    int size;    /* element count */
} Queue;

void init(Queue *q) { q->front = 0; q->rear = 0; q->size = 0; }

bool is_empty(const Queue *q) { return q->size == 0; }
bool is_full(const Queue *q)  { return q->size == MAX_SIZE; }

bool enqueue(Queue *q, int val) {
    if (is_full(q)) return false;
    q->data[q->rear] = val;
    q->rear = (q->rear + 1) % MAX_SIZE;   /* wrap around! */
    q->size++;
    return true;
}

bool dequeue(Queue *q, int *out) {
    if (is_empty(q)) return false;
    *out = q->data[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    q->size--;
    return true;
}

int front(const Queue *q) { return q->data[q->front]; }

int main(void) {
    Queue q;
    init(&q);
    printf("empty=%d\n", is_empty(&q));

    enqueue(&q, 10); enqueue(&q, 20); enqueue(&q, 30);
    printf("front=%d\n", front(&q));   /* 10 */

    int v;
    dequeue(&q, &v); printf("pop=%d\n", v);  /* 10 */
    dequeue(&q, &v); printf("pop=%d\n", v);  /* 20 */

    enqueue(&q, 40); enqueue(&q, 50); enqueue(&q, 60); /* rear 绕回下标0 */
    enqueue(&q, 70);
    printf("full=%d\n", is_full(&q));          /* 1 */
    printf("push70=%d\n", enqueue(&q, 99));    /* 0, rejected */

    while (!is_empty(&q)) { dequeue(&q, &v); printf("%d ", v); }
    printf("\n");
    return 0;
}
