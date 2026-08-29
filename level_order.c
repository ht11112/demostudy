#include <stdio.h>
#include <stdlib.h>

struct Node { int val; struct Node *left, *right; };

// ---- 循环队列（元素类型是 Node*）----
#define MAX 16
struct Node *q[MAX];
int front = 0, rear = 0, size = 0;

void enq(struct Node *n) { q[rear] = n; rear = (rear + 1) % MAX; size++; }
struct Node *deq(void)   { struct Node *n = q[front]; front = (front + 1) % MAX; size--; return n; }
int q_empty(void)        { return size == 0; }

// ---- 层序遍历 ----
void level_order(struct Node *root) {
    if (root == NULL) return;        // 坑1：空树直接返回
    enq(root);                       // 口诀第1步：根入队
    while (!q_empty()) {             // 队空结束
        struct Node *cur = deq();    // 口诀第2步：出队访问
        printf("%d ", cur->val);
        if (cur->left)  enq(cur->left);   // 口诀第3步
        if (cur->right) enq(cur->right);  // 口诀第4步
    }
    printf("\n");
}

// ---- 建树（1-2-3-4-5，与 btree_demo 同款）----
struct Node *new_node(int v) {
    struct Node *n = malloc(sizeof(struct Node));
    n->val = v; n->left = NULL; n->right = NULL;   // 坑2：左右必须置 NULL
    return n;
}

int main(void) {
    struct Node *r = new_node(1);
    r->left = new_node(2);  r->right = new_node(3);
    r->left->left = new_node(4);  r->left->right = new_node(5);
    level_order(r);   // 预期输出: 1 2 3 4 5
    return 0;
}