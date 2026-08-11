#include <stdio.h>
int main(void) {
    int a[5] = {1,2,3,4,5}, *p = a;
    printf("sizeof(a)=%zu sizeof(p)=%zu\n", sizeof(a), sizeof(p));
    printf("a=%p &a=%p p=%p\n", (void*)a, (void*)&a, (void*)p);
    printf("p+1=%p &a+1=%p\n", (void*)(p+1), (void*)(&a+1));
    return 0;
}
