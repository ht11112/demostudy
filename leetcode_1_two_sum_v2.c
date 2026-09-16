#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 64   /* 哈希表槽数，够装本题的元素即可 */

/* 哈希表的每个槽位 */
typedef struct {
    int key;    /* 存的数字：nums[i] 的值 */
    int val;    /* 这个数字的下标 i */
    int used;   /* 0 = 空槽，1 = 已占用 */
} Entry;

/* 把任意整数映射到 0 ~ TABLE_SIZE-1；先转 unsigned 防负数下标 */
static int hash_of(int key)
{
    return (int)((unsigned)key % TABLE_SIZE);
}

/* ---------- 解法一：暴力双循环，时间 O(n^2) ---------- */
static int two_sum_brute(const int *nums, int len, int target, int *out)
{
    int i;
    int j;

    for (i = 0; i < len; i++) {
        for (j = i + 1; j < len; j++) {
            if (nums[i] + nums[j] == target) {
                out[0] = i;     /* 注意：先找到的下标小 */
                out[1] = j;
                return 1;
            }
        }
    }
    return 0;
}

/* ---------- 解法二：哈希表，时间 O(n) ---------- */
static int two_sum_hash(const int *nums, int len, int target, int *out)
{
    /* calloc 会把内存清零，所以每个 used 天然是 0 */
    Entry *table = (Entry *)calloc(TABLE_SIZE, sizeof(Entry));

    int i;
    int p;

    if (table == NULL) {
        return 0;               /* 内存分配失败，按"没找到"处理 */
    }

    for (i = 0; i < len; i++) {

        /* 第 1 步：算出我要找的"另一半" */
        int need = target - nums[i];
        int start = hash_of(need);

        /* 第 2 步：先查表——之前见过的数里有没有 need？ */
        for (p = 0; p < TABLE_SIZE; p++) {
            int pos = (start + p) % TABLE_SIZE;   /* 线性探测 */

            if (table[pos].used == 0) {
                break;            /* 撞到空槽 = 表里没有 need */
            }
            if (table[pos].key == need) {
                out[0] = table[pos].val;   /* need 的下标（更小） */
                out[1] = i;                /* 当前数下标（更大） */
                free(table);
                return 1;
            }
        }

        /* 第 3 步：没查到，把当前数插进表，供后面的数来配对 */
        start = hash_of(nums[i]);
        for (p = 0; p < TABLE_SIZE; p++) {
            int pos = (start + p) % TABLE_SIZE;

            if (table[pos].used == 0) {
                table[pos].key = nums[i];
                table[pos].val = i;
                table[pos].used = 1;
                break;
            }
        }
    }

    free(table);
    return 0;
}

/* ---------- 测试框架：同一组用例喂给两个解法，结果必须一致 ---------- */
static void run_case(const int *nums, int len, int target)
{
    int got_brute[2] = { -1, -1 };
    int got_hash[2] = { -1, -1 };
    int ok_brute = two_sum_brute(nums, len, target, got_brute);
    int ok_hash = two_sum_hash(nums, len, target, got_hash);
    int same;

    /* 两个解法都必须找到，且给出的下标对必须一模一样 */
    same = (ok_brute == ok_hash) &&
           (got_brute[0] == got_hash[0]) &&
           (got_brute[1] == got_hash[1]);

    printf("len=%d target=%d | brute:[%d,%d] hash:[%d,%d]  %s\n",
           len, target,
           got_brute[0], got_brute[1],
           got_hash[0], got_hash[1],
           same ? "PASS" : "FAIL");
}

int main(void)
{
    int case1[] = { 2, 7, 11, 15 };          /* 经典用例 */
    int case2[] = { 3, 2, 4 };               /* 答案不含自己 */
    int case3[] = { 3, 3 };                  /* 两个相同的数！ */
    int case4[] = { -3, 4, 3, 90 };          /* 有负数 */
    int case5[] = { -1, -2, -3, -4, -5 };    /* 全负数，target 也是负数 */

    run_case(case1, 4, 9);
    run_case(case2, 3, 6);
    run_case(case3, 2, 6);
    run_case(case4, 4, 0);
    run_case(case5, 5, -8);

    return 0;
}
