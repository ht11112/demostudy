#include <stdio.h>
#include <stdlib.h>


void bubble_sort(int a[], int n)
{
    int i;
    int j;

    for (i = 0; i < n - 1; i++) {
        /* 尾部 i 个已经排好，只需比较前 n-1-i 个 */
        for (j = 0; j < n - 1 - i; j++) {
            if (a[j] > a[j + 1]) {
                int tmp = a[j];      /* 交换必须借 tmp 中转 */
                a[j] = a[j + 1];
                a[j + 1] = tmp;
            }
        }
    }
}

void selection_sort(int a[], int n)
{
    int i;
    int j;
    int min_pos;

    for (i = 0; i < n - 1; i++) {
        min_pos = i;                  /* 先假设 a[i] 最小 */
        for (j = i + 1; j < n; j++) {
            if (a[j] < a[min_pos]) {
                min_pos = j;          /* 记录真正的最小值下标 */
            }
        }
        if (min_pos != i) {           /* 本轮只交换这一次 */
            int tmp = a[i];
            a[i] = a[min_pos];
            a[min_pos] = tmp;
        }
    }
}

/* a、b 是「指向待比较元素的指针」，所以是 const void *
 * 返回 <0 → a 排前面；=0 → 相等；>0 → a 排后面 */
int cmp_int_asc(const void *a, const void *b)
{
    int va = *(const int *)a;   /* 先转回 int*，再取值 */
    int vb = *(const int *)b;

    /* 坑：不要写 return va - vb;  va=2e9、vb=-2e9 相减会溢出 int */
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}
/* 调用：qsort(a_qsort, n, sizeof(int), cmp_int_asc); */


int binary_search(const int a[], int n, int target)
{
    int left = 0;
    int right = n - 1;

    while (left <= right) {                  /* ① 必须有等号 */
        int mid = left + (right - left) / 2; /* ② 不能写 (left+right)/2 */
        if (a[mid] == target) {
            return mid;
        } else if (a[mid] < target) {
            left = mid + 1;                  /* ③ 必须 ±1，写 mid 会死循环 */
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

/* 和 qsort 的比较函数签名一样，给 bsearch 用 */
int cmp_for_bsearch(const void *key, const void *elem)
{
    int k = *(const int *)key;
    int v = *(const int *)elem;

    if (k < v) return -1;
    if (k > v) return 1;
    return 0;
}

void print_array(const char *tag, const int a[], int n)
{
    int i;

    printf("%-14s", tag);
    for (i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}



int main(void)
{
    /* 换成含负数 + 重复值的 12 个数据，才能压出比较逻辑的错 */
    int origin[] = { 42, 7, -3, 99, 0, 7, 15, -20, 88, 1, 56, 3 };
    int n = (int)(sizeof(origin) / sizeof(origin[0]));
    int a_bubble[12];
    int a_select[12];
    int a_qsort[12];
    int i;

    /* 关键：复制三份，保证三个算法从同一份乱数据出发 */
    for (i = 0; i < n; i++) {
        a_bubble[i] = origin[i];
        a_select[i] = origin[i];
        a_qsort[i]  = origin[i];
    }

    printf("=== 1) three sorts on the same data ===\n");
    print_array("origin:", origin, n);

    bubble_sort(a_bubble, n);
    selection_sort(a_select, n);
    qsort(a_qsort, n, sizeof(int), cmp_int_asc);

    print_array("bubble:", a_bubble, n);
    print_array("selection:", a_select, n);
    print_array("qsort:", a_qsort, n);

    /* 对拍：三种排序结果必须完全一致 */
    {
        int same = 1;
        for (i = 0; i < n; i++) {
            if (a_bubble[i] != a_select[i] || a_bubble[i] != a_qsort[i]) {
                same = 0;
                break;
            }
        }
        printf("all three equal? %s\n\n", same ? "PASS" : "FAIL");
    }

    /* ---------- 2) 手写二分 vs 标准库 bsearch ---------- */
    printf("=== 2) binary search vs bsearch ===\n");
    {
        int targets[] = { 42, -20, 7, 99, 0, 100, -999 };   /* 后两个不存在 */
        int tn = (int)(sizeof(targets) / sizeof(targets[0]));
        int t;

        for (t = 0; t < tn; t++) {
            int key = targets[t];
            int mine = binary_search(a_qsort, n, key);
            /* bsearch 返回"指向该元素的指针"，找不到返回 NULL */
            int *found = (int *)bsearch(&key, a_qsort, n, sizeof(int), cmp_for_bsearch);
            int lib = found ? (int)(found - a_qsort) : -1;   /* 指针相减 = 下标 */

            printf("find %5d -> mine: %3d  bsearch: %3d  %s\n",
                   key, mine, lib, (mine == lib) ? "PASS" : "FAIL");
        }
    }

    /* ---------- 3) 边界 ---------- */
    printf("\n=== 3) edge cases ===\n");
    {
        int one[1] = { 5 };
        int empty[1] = { 0 };

        printf("empty array, find 5 -> %d (expect -1)\n", binary_search(empty, 0, 5));
        printf("single [5], find 5  -> %d (expect 0)\n", binary_search(one, 1, 5));
        printf("single [5], find 9  -> %d (expect -1)\n", binary_search(one, 1, 9));
    }

    return 0;   /* ← 必须放最后 */
}
