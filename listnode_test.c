#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* prev;   /* 多一条向后的链 */
    struct Node* next;
};

struct Node* list_create(void) {           /* 造头结点（哨兵） */
    struct Node* head = (struct Node*)malloc(sizeof(struct Node));
    if (head == NULL) { printf("malloc failed\n"); return NULL; }
    head->prev = NULL;
    head->next = NULL;
    return head;
}

void list_insert_head(struct Node* head, int val) {   /* 插到头结点后面 */
    struct Node* n = (struct Node*)malloc(sizeof(struct Node));
    if (n == NULL) { printf("malloc failed\n"); return; }
    n->data = val;
    n->prev = head;                /* 1. 新节点先连好自己两条链 */
    n->next = head->next;
    if (head->next != NULL)        /* 2. 原第一个节点回指（非空才回指） */
        head->next->prev = n;
    head->next = n;                /* 3. 最后才改头结点的链 */
}

void list_delete(struct Node* head, int val) {
    struct Node* p = head->next;   /* 从第一个真实节点开始找 */
    while (p != NULL && p->data != val)
        p = p->next;
    if (p == NULL) { printf("not found: %d\n", val); return; }
    p->prev->next = p->next;       /* 前面跳过 p */
    if (p->next != NULL)           /* 后面回指 p 的前一个 */
        p->next->prev = p->prev;
    free(p);
}

void list_print(struct Node* head) {
    printf("fwd: ");
    for (struct Node* p = head->next; p != NULL; p = p->next)
        printf("%d ", p->data);
    printf("NULL\n");
}

void list_print_rev(struct Node* head) {
    struct Node* p = head->next;
    if (p == NULL) { printf("rev: NULL\n"); return; }
    while (p->next != NULL) p = p->next;   /* 先走到最后一个 */
    printf("rev: ");
    while (p != head) {                    /* 一路 prev 走回头结点为止 */
        printf("%d ", p->data);
        p = p->prev;
    }
    printf("NULL\n");
}

void list_destroy(struct Node* head) {
    struct Node* p = head;
    while (p != NULL) {
        struct Node* tmp = p->next;
        free(p);
        p = tmp;
    }
}

int main(void) {
    struct Node* head = list_create();
    if (head == NULL) return 1;
    list_insert_head(head, 10);
    list_insert_head(head, 20);
    list_insert_head(head, 30);
    list_print(head);       /* fwd: 30 20 10 NULL */
    list_print_rev(head);   /* rev: 10 20 30 NULL */
    list_delete(head, 20);
    list_print(head);       /* fwd: 30 10 NULL */
    list_print_rev(head);   /* rev: 10 30 NULL */
    list_destroy(head);
    return 0;
}
