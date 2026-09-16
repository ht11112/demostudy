#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 128

typedef struct { int key; int used; } E;

static int hash_of(int k)
{
    return (int)((unsigned)k % TABLE_SIZE);
}

/* hash version: O(n) time, O(n) space */
static int contains_duplicate(const int *nums, int n)
{
    E *tbl = (E *)calloc(TABLE_SIZE, sizeof(E));
    int i, p;

    if (!tbl) return -1;

    for (i = 0; i < n; i++) {
        int start = hash_of(nums[i]);
        for (p = 0; p < TABLE_SIZE; p++) {
            int pos = (start + p) % TABLE_SIZE;
            if (!tbl[pos].used) {
                tbl[pos].key  = nums[i];
                tbl[pos].used = 1;
                break;
            }
            if (tbl[pos].key == nums[i]) {
                free(tbl);
                return 1;               /* duplicate found */
            }
        }
    }

    free(tbl);
    return 0;
}

/* qsort fallback: O(n log n) time, O(1) extra space (besides sort) */
static int cmp_int(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);          /* overflow-safe */
}

static int contains_duplicate_sort(int *nums, int n)
{
    int i;
    if (n < 2) return 0;
    qsort(nums, (size_t)n, sizeof(int), cmp_int);
    for (i = 1; i < n; i++)
        if (nums[i] == nums[i - 1]) return 1;
    return 0;
}

static void run_case(const int *src, int n, int expect)
{
    int *copy = (int *)malloc(sizeof(int) * (size_t)(n > 0 ? n : 1));
    int got, got2;
    int i;

    for (i = 0; i < n; i++) copy[i] = src[i];
    got = contains_duplicate(src, n);
    got2 = contains_duplicate_sort(copy, n);

    printf("n=%d | hash:%d sort:%d expect:%d  %s\n",
           n, got, got2, expect,
           (got == expect && got2 == expect) ? "PASS" : "FAIL");
    free(copy);
}

int main(void)
{
    int a[] = { 1, 2, 3, 1 };
    int b[] = { 1, 2, 3, 4 };
    int c[] = { 1, 1, 1, 3, 3, 4, 3, 2, 4, 2 };
    int d[] = { 7 };
    int e[] = { -1, -1 };
    int f[] = { 0, 0 };

    run_case(a, 4, 1);
    run_case(b, 4, 0);
    run_case(c, 10, 1);
    run_case(d, 1, 0);
    run_case(e, 2, 1);
    run_case(f, 2, 1);

    return 0;
}
