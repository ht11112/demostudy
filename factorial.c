#include <stdio.h>

/* factorial: n! = n * (n-1)! , base case 0! = 1 */
int factorial(int n)
{
    if (n <= 1) {        /* stop condition: 出口 */
        return 1;
    }
    return n * factorial(n - 1);   /* recursive step: 递推关系 */
}

int main(void)
{
    int i;
    for (i = 0; i <= 6; i++) {
        printf("fact(%d) = %d\n", i, factorial(i));
    }
    return 0;
}
