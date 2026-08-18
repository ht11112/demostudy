#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* next;
};

/* 在堆上造一个节点：malloc + 判空 + 初始化 */
struct Node* create_node(int val) {
    struct Node* n = (struct Node*)malloc(sizeof(struct Node));
    if (n == NULL) { printf("malloc failed\n"); return NULL; }
    n->data = val;
    n->next = NULL;
    return n;
}

/* 释放整条链：先存 tmp，再 free 当前，再往后走 */
void destroy_list(struct Node* head) {
    struct Node* p = head;
    while (p != NULL) {
        struct Node* tmp = p->next;   /* 先记住下一个 */
        printf("free %d\n", p->data);
        free(p);                      /* 再释放当前 */
        p = tmp;                      /* 再移动 */
    }
}

int main(void) {
    struct Node* head = NULL, *tail = NULL;
    for (int i = 1; i <= 3; i++) {
        struct Node* n = create_node(i * 10);
        if (n == NULL) return 1;
        if (head == NULL) { head = tail = n; }     /* 第一个节点 */
        else { tail->next = n; tail = n; }         /* 尾插法 */
    }
    for (struct Node* p = head; p != NULL; p = p->next)
        printf("data=%d\n", p->data);
    destroy_list(head);
    head = NULL;      /* 防悬空指针：free 后指针要置 NULL */
    return 0;
}