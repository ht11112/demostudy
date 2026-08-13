#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* next;   /* 自引用指针：链表的基石 */
};

int main(void)
{
    struct Node n1, n2, n3;
    n1.data = 10; n2.data = 20; n3.data = 30;
    n1.next = &n2;
    n2.next = &n3;
    n3.next = NULL;       /* 结尾必须置 NULL，否则遍历越界 */

    struct Node* p = &n1;
    while (p != NULL) {
        printf("data=%d\n", p->data);   /* p->data 等价 (*p).data */
        p = p->next;
    }
    return 0;
}
