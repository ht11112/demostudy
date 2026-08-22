#include <stdio.h>
#include <stdbool.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;              // top element index, -1 = empty
} Stack;

void init(Stack *s) {
    s->top = -1;
}

bool is_empty(const Stack *s) {
    return s->top == -1;
}

bool is_full(const Stack *s) {
    return s->top == MAX_SIZE - 1;
}

bool push(Stack *s, int val) {
    if (is_full(s)) {
        printf("[push] full, reject %d\n", val);
        return false;
    }
    s->top++;              // step1: move up
    s->data[s->top] = val; // step2: then put
    return true;
}

bool pop(Stack *s, int *out) {
    if (is_empty(s)) {
        printf("[pop] empty\n");
        return false;
    }
    *out = s->data[s->top]; // step1: take first
    s->top--;               // step2: then move down
    return true;
}

bool top(const Stack *s, int *out) {
    if (is_empty(s)) return false;
    *out = s->data[s->top];
    return true;
}

int main(void) {
    Stack s;
    init(&s);

    printf("empty = %d\n", is_empty(&s));      // expect 1

    push(&s, 10);
    push(&s, 20);
    push(&s, 30);

    int val;
    top(&s, &val);
    printf("top = %d\n", val);                 // expect 30
   
    while (!is_empty(&s)) {
        pop(&s, &val);
        printf("pop = %d\n", val);             // expect 30 20 10
    }
    return 0;
}
