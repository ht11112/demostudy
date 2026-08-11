#include <stdio.h>
typedef int SLTDataType;
typedef struct SListNode { SLTDataType data; struct SListNode* next; } SLTNode;

int main(void) {
    SLTNode n1;            // 用小名声明，等价于 struct SListNode n1;
    struct SListNode n2;   // 用全名声明，两个是同一个类型
    n1.data = 10;
    n2.data = 20;
    printf("两个名字是同一个类型: %d %d, 大小=%zu\n",
           n1.data, n2.data, sizeof(SLTNode));
    return 0;
}
