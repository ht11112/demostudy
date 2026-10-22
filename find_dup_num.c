#include <stdio.h>
#include <string.h>

/*
 * Find ALL duplicate numbers in an int array.
 * Method: bitmap (one bit per value). Fast, but needs value range <= BIT_MAX.
 */
#define VALUE_MAX 64          /* values must be 0..63 */
#define BIT_WORDS 1           /* 64 bits = one unsigned long long */

/* set bit -> already 1 means duplicate, return 1 */
static int bitmap_set(unsigned long long *bm, int value)
{
    unsigned long long mask = 0ULL;

    mask = 1ULL << (value & 63);
    if ((*bm & mask) != 0ULL) {
        return 1;             /* seen before */
    }
    *bm = *bm | mask;
    return 0;
}

/* return number of duplicates, fill them into out[] */
static int find_dup_bitmap(const int *a, int n, int *out)
{
    unsigned long long bm = 0ULL;
    int count = 0;
    int i;

    for (i = 0; i < n; i++) {
        if (bitmap_set(&bm, a[i]) == 1) {
            int j;
            int already = 0;

            /* avoid reporting the same value twice */
            for (j = 0; j < count; j++) {
                if (out[j] == a[i]) {
                    already = 1;
                }
            }
            if (already == 0) {
                out[count] = a[i];
                count++;
            }
        }
    }
    return count;
}

/* counting array version: works for any non-negative range, O(V) space */
static int find_dup_counting(const int *a, int n, int *out)
{
    static int seen[VALUE_MAX];   /* static: zero-init, keeps stack small on MCU */
    int count = 0;
    int i;

    memset(seen, 0, sizeof(seen));
    for (i = 0; i < n; i++) {
        seen[a[i]]++;
    }
    for (i = 0; i < VALUE_MAX; i++) {
        if (seen[i] > 1) {
            out[count] = i;
            count++;
        }
    }
    return count;
}

static void print_result(const char *tag, const int *out, int count)
{
    int i;

    printf("%s -> count=%d :", tag, count);
    for (i = 0; i < count; i++) {
        printf(" %d", out[i]);
    }
    printf("\n");
}

int main(void)
{
    int a1[] = { 3, 1, 4, 1, 5, 9, 2, 6, 5, 3 };
    int a2[] = { 0, 7, 7, 7, 63 };
    int a3[] = { 10, 20, 30 };
    int out1[16];
    int out2[16];
    int out3[16];
    int c1;
    int c2;
    int c3;

    /* case 1: duplicates 1, 3, 5 */
    c1 = find_dup_bitmap(a1, 10, out1);
    print_result("bitmap a1", out1, c1);
    if (c1 == 3 && out1[0] == 1 && out1[1] == 5 && out1[2] == 3) {
        printf("  PASS\n");
    } else {
        printf("  FAIL\n");
    }

    /* case 2: boundary values 0 and 63, duplicate 7 counted once */
    c2 = find_dup_bitmap(a2, 5, out2);
    print_result("bitmap a2", out2, c2);
    if (c2 == 1 && out2[0] == 7) {
        printf("  PASS\n");
    } else {
        printf("  FAIL\n");
    }

    /* case 3: no duplicate */
    c3 = find_dup_bitmap(a3, 3, out3);
    print_result("bitmap a3", out3, c3);
    if (c3 == 0) {
        printf("  PASS\n");
    } else {
        printf("  FAIL\n");
    }

    /* cross check: bitmap vs counting array on a1 */
    printf("\n-- cross check on a1 --\n");
    {
        int out_c[16];
        int cc;

        cc = find_dup_counting(a1, 10, out_c);
        print_result("counting a1", out_c, cc);
        if (cc == c1 && out_c[0] == 1 && out_c[1] == 3 && out_c[2] == 5) {
            printf("  PASS\n");
        } else {
            printf("  FAIL\n");
        }
    }

    return 0;
}
