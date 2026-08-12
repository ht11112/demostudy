#include <stdio.h>

void func(int n) {
    int local = n;   // 每次调用都是全新的一份
    printf("depth %d: local addr = %p\n", n, (void *)&local)
    if (n > 0) {
        func(n - 1);   // 递归调用
    }
}

int main(void) {
    func(5);
    return 0;
}
