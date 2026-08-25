/*
 * LeetCode 225: Implement Stack using Queues
 * Approach: single queue rotate method
 * After each push, rotate old elements to the tail,
 * so the queue head is always the stack top.
 *
 * Compile: gcc -Wall -o my_queue_to_stack.exe my_queue_to_stack.c && ./my_queue_to_stack.exe
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX 100

typedef struct {
    int data[MAX];
    int front;   /* queue head index */
    int rear;    /* queue tail index (next empty slot) */
    int size;    /* element count */
} MyStack;

MyStack* myStackCreate() {
    MyStack* s = (MyStack*)malloc(sizeof(MyStack));
    s->front = 0;
    s->rear = 0;
    s->size = 0;
    return s;
}

void myStackPush(MyStack* obj, int x) {
    /* step 1: enqueue new element at tail */
    obj->data[obj->rear] = x;
    obj->rear = (obj->rear + 1) % MAX;
    obj->size++;

    /* step 2: rotate (size-1) old elements to the tail,
       so new element becomes the head = stack top */
    for (int i = 1; i < obj->size; i++) {
        int v = obj->data[obj->front];
        obj->front = (obj->front + 1) % MAX;
        obj->data[obj->rear] = v;
        obj->rear = (obj->rear + 1) % MAX;
    }
}

int myStackPop(MyStack* obj) {
    int v = obj->data[obj->front];
    obj->front = (obj->front + 1) % MAX;
    obj->size--;
    return v;
}

int myStackTop(MyStack* obj) {
    return obj->data[obj->front];
}

bool myStackEmpty(MyStack* obj) {
    return obj->size == 0;
}

void myStackFree(MyStack* obj) {
    free(obj);
}

int main(void) {
    MyStack* s = myStackCreate();

    myStackPush(s, 1);
    myStackPush(s, 2);
    myStackPush(s, 3);

    printf("top   = %d (expect 3)\n", myStackTop(s));
    printf("pop   = %d (expect 3)\n", myStackPop(s));
    printf("pop   = %d (expect 2)\n", myStackPop(s));
    printf("empty = %d (expect 0)\n", myStackEmpty(s));

    myStackPush(s, 9);
    printf("top   = %d (expect 9)\n", myStackTop(s));
    printf("empty = %d (expect 0)\n", myStackEmpty(s));

    myStackFree(s);
    return 0;
}