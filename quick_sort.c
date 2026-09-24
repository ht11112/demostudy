/*
 * quick_sort.c —— 第 6 周（C 语言收官周）排序进阶：手写快速排序
 *
 * 为什么嵌入式也要会手写快排？
 *   1) 面试高频：手撕快排 + 讲清 partition 是基本要求；
 *   2) 嵌入式里 qsort 会引入递归和函数指针调用开销，
 *      有时需要自己写一个"数据量小、可控"的排序；
 *   3) partition（分区）思想本身就是很多算法的基础
 *      （快速选择找第 K 大、荷兰国旗问题、后续的 TOP-K 都用它）。
 *
 * 本文件用最经典的 Lomuto 分区法（好懂、好背）：
 *   选最后一个元素当"基准 pivot"
 *   用一个 i 指向"小于等于 pivot 区域的最后一个位置"
 *   j 从左扫到右，遇到比 pivot 小的就把它换到 i+1 处，i 后移
 *   扫描结束后把 pivot 换到 i+1 —— 此时 pivot 就位，左边全 <= 它，右边全 > 它
 *
 * 编译命令：
 *   gcc -Wall -Wextra -O2 quick_sort.c -o quick_sort.exe
 *   ./quick_sort.exe
 */

#include <stdio.h>
#include <stdlib.h>

/* ============================================================
 * 交换两个整数
 * 为什么单独写函数？因为快排里交换出现 2 处，抽出来更好读。
 * 注意：C 语言函数改不了实参，所以必须传"地址"（int *）
 * ============================================================ */
void swap_int(int *a, int *b)
{
    int tmp = *a;   /* 先把 a 指向的值存到临时变量 */
    *a = *b;        /* 把 b 指向的值写进 a 指向的位置 */
    *b = tmp;       /* 再把临时变量写回 b 指向的位置 */
}

/* ============================================================
 * Lomuto 分区函数
 *
 * 参数：
 *   a     —— 待分区的数组
 *   left  —— 本段最左下标
 *   right —— 本段最右下标（这里把它当 pivot 的位置）
 *
 * 返回值：pivot 最终落位的下标（这个位置排序后就是它最终位置）
 * ============================================================ */
int partition(int a[], int left, int right)
{
    int pivot = a[right];   /* 基准取最后一个元素 */
    int i = left - 1;       /* i 是"小值区"的右边界，初始为空（left-1） */
    int j;

    for (j = left; j < right; j++) {
        if (a[j] <= pivot) {
            i++;                    /* 小值区扩张一格 */
            swap_int(&a[i], &a[j]); /* 把 a[j] 挪进小值区 */
        }
    }

    /* 循环结束：a[left..i] 全部 <= pivot，a[i+1..right-1] 全部 > pivot
     * 现在把 pivot 从 right 换到 i+1，它就到位了 */
    swap_int(&a[i + 1], &a[right]);
    return i + 1;
}

/* ============================================================
 * 快速排序主函数（递归）
 *
 * 递归的"出口"：left >= right 表示这一段只有 0 个或 1 个元素，
 *               天然有序，直接 return —— 少了这句会无限递归、栈溢出！
 * ============================================================ */
void quick_sort(int a[], int left, int right)
{
    int p;   /* pivot 落位后的下标 */

    if (left >= right) {
        return;
    }

    p = partition(a, left, right);   /* 先分区，pivot 就位 */
    quick_sort(a, left, p - 1);      /* 再递归排左半段 */
    quick_sort(a, p + 1, right);     /* 再递归排右半段 */
}

/* ============================================================
 * 和标准库 qsort 对拍用的比较函数（必须防 int 相减溢出）
 * ============================================================ */
int cmp_int_asc(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;

    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

/* 打印数组 */
void print_array(const char *tag, const int a[], int n)
{
    int i;

    printf("%-12s", tag);
    for (i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

/* ============================================================
 * 通用测试：把同一份乱数据分别交给"手写快排"和"库 qsort"，
 * 逐个元素比对，全一致才算 PASS。
 * 这一步很关键 —— 只看输出"看起来有序"不算验证。
 * ============================================================ */
int check_case(const char *name, int src[], int n)
{
    int *mine;
    int *lib;
    int i;
    int ok = 1;

    if (n == 0) {
        /* 空数组单独处理：不申请内存，直接算通过 */
        printf("%-22s n=0   -> PASS (nothing to sort)\n", name);
        return 1;
    }

    mine = (int *)malloc(sizeof(int) * (size_t)n);
    lib  = (int *)malloc(sizeof(int) * (size_t)n);
    if (mine == NULL || lib == NULL) {
        printf("%-22s malloc failed\n", name);
        free(mine);
        free(lib);
        return 0;
    }

    for (i = 0; i < n; i++) {
        mine[i] = src[i];
        lib[i]  = src[i];
    }

    quick_sort(mine, 0, n - 1);                    /* 我的手写快排 */
    qsort(lib, (size_t)n, sizeof(int), cmp_int_asc); /* 标准库 */

    for (i = 0; i < n; i++) {
        if (mine[i] != lib[i]) {
            ok = 0;
            break;
        }
    }

    printf("%-22s n=%-3d -> %s\n", name, n, ok ? "PASS" : "FAIL");
    if (ok == 0) {
        print_array("  mine:", mine, n);
        print_array("  lib :", lib, n);
    }

    free(mine);
    free(lib);
    return ok;
}

int main(void)
{
    int all_pass = 1;

    /* ---------- 1) 基础演示：把分区过程打出来 ---------- */
    {
        int demo[] = { 42, 7, -3, 99, 0, 7, 15, -20, 88, 1, 56, 3 };
        int n = (int)(sizeof(demo) / sizeof(demo[0]));
        int p;

        printf("=== 1) one partition step (pivot = last element) ===\n");
        print_array("before:", demo, n);
        p = partition(demo, 0, n - 1);
        printf("pivot lands at index %d (value %d)\n", p, demo[p]);
        print_array("after:", demo, n);
        printf("check left <= pivot <= right ...\n");

        /* 校验分区性质：左边全部 <= pivot，右边全部 > pivot */
        {
            int i;
            int good = 1;
            for (i = 0; i < p; i++) {
                if (demo[i] > demo[p]) good = 0;
            }
            for (i = p + 1; i < n; i++) {
                if (demo[i] < demo[p]) good = 0;
            }
            printf("partition property: %s\n\n", good ? "PASS" : "FAIL");
            if (!good) all_pass = 0;
        }
    }

    /* ---------- 2) 六组用例对拍 ---------- */
    printf("=== 2) compare with stdlib qsort ===\n");
    {
        int c1[] = { 5, 2, 9, 1, 5, 6 };
        int c2[] = { 1 };
        int c3[] = { 3, 3, 3, 3, 3 };                       /* 全相同 */
        int c4[] = { 1, 2, 3, 4, 5, 6, 7, 8 };              /* 已升序（最坏情况） */
        int c5[] = { 8, 7, 6, 5, 4, 3, 2, 1 };              /* 逆序 */
        int c6[] = { 2147483647, -2147483648, 0, -1, 1 };   /* int 极值 */
        int empty[1] = { 0 };                               /* 占位，n 传 0 */

        if (!check_case("random 6", c1, 6)) all_pass = 0;
        if (!check_case("single", c2, 1)) all_pass = 0;
        if (!check_case("all equal", c3, 5)) all_pass = 0;
        if (!check_case("already sorted", c4, 8)) all_pass = 0;
        if (!check_case("reverse sorted", c5, 8)) all_pass = 0;
        if (!check_case("int extremes", c6, 5)) all_pass = 0;
        if (!check_case("empty", empty, 0)) all_pass = 0;
    }

    /* ---------- 3) 大规模对拍：10000 个随机数 ---------- */
    printf("\n=== 3) stress test: 10000 random ints ===\n");
    {
        int n = 10000;
        int *big = (int *)malloc(sizeof(int) * (size_t)n);
        int i;
        int seed = 12345;

        if (big == NULL) {
            printf("malloc failed\n");
            all_pass = 0;
        } else {
            /* 自己写个线性同余随机数，避免 rand() 在不同平台结果不一致 */
            for (i = 0; i < n; i++) {
                seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
                big[i] = seed % 20001 - 10000;   /* 范围 -10000 ~ 10000 */
            }
            if (!check_case("random 10000", big, n)) all_pass = 0;
            free(big);
        }
    }

    printf("\n=== SUMMARY: %s ===\n", all_pass ? "ALL PASS" : "SOME FAILED");
    return all_pass ? 0 : 1;
}
