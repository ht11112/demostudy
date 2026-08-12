#include <stdio.h>
#include <stdlib.h>

int g_global = 10;              // 全局变量 → 静态区
static int s_static = 20;       // 静态局部 → 静态区

int main(void) {
    int a = 5;                  // 局部变量 → 栈
    static int s_local = 30;    // 静态局部 → 静态区
    int *p = (int *)malloc(sizeof(int));  // malloc 的内存 → 堆
    *p = 40;
    const char *str = "hello";  // 字符串常量 → 只读区（代码区）

    printf("code  string const: %p\n", (void *)str);
    printf("static global:      %p\n", (void *)&g_global);
    printf("static local:       %p\n", (void *)&s_local);
    printf("heap   malloc:      %p\n", (void *)p);
    printf("stack  local:       %p\n", (void *)&a);


    free(p);   // malloc 必须配对 free
    return 0;
}
