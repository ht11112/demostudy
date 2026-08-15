#include <stdio.h>
#include <stdlib.h>   /* malloc / free 的函数声明在这个头文件里 */

int main(void)
{
    /* 1. malloc 一个 int：在堆上要 sizeof(int) 个字节，返回首地址 */
    int* p = (int*)malloc(sizeof(int));
    if (p == NULL) {              /* 分配失败会返回 NULL，必须检查！ */
        printf("malloc failed\n");
        return 1;
    }
    *p = 42;                      /* 通过指针往堆里的内存写值 */
    printf("p=%p *p=%d\n", (void*)p, *p);

    /* 2. malloc 一个 10 元素的 int 数组 */
    int n = 10;
    int* arr = (int*)malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("malloc failed\n");
        return 1;
    }
    for (int i = 0; i < n; i++)
        arr[i] = i * i;           /* 用起来和普通数组一样 */
    printf("arr[5]=%d\n", arr[5]);

    /* 3. 用完了，退租 */
    free(p);
    free(arr);
    return 0;
}