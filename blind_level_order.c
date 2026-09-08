/* blind_level_order.c - Level-order traversal (BFS), BLIND TEST
 * Fill in the TODO parts below from memory.
 * Do NOT open level_order.c until you finish writing.
 */

#include <stdio.h>
#include <stdlib.h>

struct Node {
    int val;
    struct Node *left;
    struct Node *right;
};

struct Node *new_node(int v) {
    struct Node *n = malloc(sizeof(struct Node));
    n->val = v;
    n->left = NULL;
    n->right = NULL;
    return n;
}

/* ===== FILL IN 1: global circular queue (element type: struct Node *) ===== */
#define MAX 16
struct Node *q[MAX];
int front = 0, rear = 0, size = 0;

void enq(struct Node *n) {
    q[rear] = n;
    rear = (rear + 1) % MAX;
    size++;
}

struct Node *deq(void) {
    struct Node *n = q[front];
    front = (front + 1) % MAX;
    size--;
    return n;
}
int q_empty(void){
    return size ==0;
}


/* TODO: declare 4 globals below:
 *   struct Node *q[MAX];   (queue storage)dak
 *   int front = 0, rear = 0, size = 0;
 */

/* TODO: void enq(struct Node *n)
 *   - store n at q[rear]
 *   - rear = (rear + 1) % MAX
 *   - size++;
 */

/* TODO: struct Node *deq(void)
 *   - take n = q[front]
 *   - front = (front + 1) % MAX
 *   - size--;
 *   - return n;
 */

/* TODO: int q_empty(void)  -> return 1 if empty, 0 otherwise */

/* ===== FILL IN 2: level_order - four-step rhythm =====
 * 1) if root == NULL, just return
 * 2) enq(root)
 * 3) while queue NOT empty:
 *      cur = deq();  printf("%d ", cur->val);
 *      if cur->left  -> enq(cur->left);
 *      if cur->right -> enq(cur->right);
 * 4) print a newline at the end
 */
void level_order(struct Node *root) {
    /* TODO: write it using the four-step rhythm above */
    if (root == NULL) return;
    enq(root);
    while (!q_empty()) {
        struct Node *cur = deq();
        printf("%d ", cur->val);
        if (cur->left) enq(cur->left);
        if (cur->right) enq(cur->right);
    }
    printf("\n");
}

int main(void) {
    struct Node *r = new_node(1);
    r->left = new_node(2);  r->right = new_node(3);
    r->left->left = new_node(4);  r->left->right = new_node(5);
    level_order(r);   /* expected output: 1 2 3 4 5 */
    return 0;
}
