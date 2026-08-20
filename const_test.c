/* const_test.c - const keyword experiment (8/20)
 * Try uncommenting the ERROR lines one by one and recompile
 * to see the compiler reject them. */

#include <stdio.h>

void print_arr(const int *arr, int n) {   /* const param: promise not to modify */
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main(void) {
    int a = 10, b = 20;

    const int score = 100;
    /* score = 99;        // ERROR: assignment of read-only variable */

    const int *p1 = &a;   /* value read-only, pointer movable */
    p1 = &b;              /* OK */
    printf("*p1=%d\n", *p1);
    /* *p1 = 99;         // ERROR: assignment of read-only location */

    int *const p2 = &a;   /* pointer read-only, value mutable */
    *p2 = 99;             /* OK */
    /* p2 = &b;          // ERROR: assignment of read-only variable 'p2' */

    int arr[3] = {1, 2, 3};
    print_arr(arr, 3);    /* OK: pass non-const to const param */

    printf("a=%d score=%d\n", a, score);
    return 0;
}
