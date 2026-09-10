#include <stdio.h>
#include <time.h>

int plain_flag = 0;            /* 普通变量 */
volatile int vol_flag = 0;     /* volatile 变量 */

int main(void) {
    clock_t t0, t1;
    long i;

    t0 = clock();
    for (i = 0; i < 200000000L; i++) plain_flag = 1;   /* 预期被优化掉 */
    t1 = clock();
    printf("plain   : ticks=%ld\n", (long)(t1 - t0));

    t0 = clock();
    for (i = 0; i < 200000000L; i++) vol_flag = 1;     /* 预期保留 */
    t1 = clock();
    printf("volatile: ticks=%ld\n", (long)(t1 - t0));
    return 0;
}
