#include <stdio.h>

/*
 * LC 136 Single Number: every element appears twice except one.
 * Trick: XOR all -> pairs cancel out, the single one remains.
 *   5 ^ 5 = 0,  0 ^ x = x,  XOR is commutative.
 */
static int single_number(const int *nums, int n)
{
    int result = 0;
    int i;

    for (i = 0; i < n; i++) {
        result = result ^ nums[i];
    }
    return result;
}

static void run_case(const char *tag, const int *nums, int n, int expect)
{
    int got = single_number(nums, n);

    printf("%s -> got %d (expect %d) %s\n",
           tag, got, expect, got == expect ? "PASS" : "FAIL");
}

int main(void)
{
    int a1[] = { 2, 2, 1 };
    int a2[] = { 4, 1, 2, 1, 2 };
    int a3[] = { 1 };
    int a4[] = { -1, -1, -7 };

    run_case("case1", a1, 3, 1);
    run_case("case2", a2, 5, 4);
    run_case("case3", a3, 1, 1);
    run_case("case4", a4, 3, -7);   /* negative numbers: XOR works on bit patterns */

    return 0;
}
