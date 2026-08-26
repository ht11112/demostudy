#include <stdio.h>

/* fib: fib(n) = fib(n-1) + fib(n-2), base fib(0)=0 fib(1)=1 */
int fib(int n)
{
    if (n <= 1) {          /* stop condition */
        return n;
    }
    return fib(n - 1) + fib(n - 2);   /* recursive step */
}

int main(void)
{
    int i;
    for (i = 0; i <= 10; i++) {
        printf("fib(%d) = %d\n", i, fib(i));
    }
    return 0;
}
