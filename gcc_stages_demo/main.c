#include <stdio.h>

/*
 * gcc_stages_demo/main.c —— 演示 GCC 编译四阶段
 *
 * 这个文件故意同时用到"宏"和"头文件"，这样四个阶段各自的产物差异才看得出来。
 */

#define GREET_TAG "gcc-stages"

#ifndef GREET_TAG
#error "GREET_TAG missing"
#endif

int add(int a, int b)
{
    return a + b;
}

int main(void)
{
    int x = 3;
    int y = 4;

    printf("[%s] %d + %d = %d\n", GREET_TAG, x, y, add(x, y));
    return 0;
}
