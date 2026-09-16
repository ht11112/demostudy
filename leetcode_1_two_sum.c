/* 力扣 1：两数之和（哈希表 O(n) 版）
 * 铁律：先查 need，再插入自己。顺序反了会「自己和自己配对」。
 *       例：target = 6，nums = [3, 2, 4]，扫到第一个 3 就会返回 [0,0]，错。
 */
#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 13   /* 用素数当槽位数，取模后分布更均匀 */

typedef struct {
    int key;     /* 存数组里的数值 */
    int value;   /* 存这个数值对应的下标 */
    int used;    /* 1 = 槽位被占用；0 = 空 */
} Slot;

/* 把 key 映射成 [0, TABLE_SIZE-1] 之间的槽位下标 */
static int hash_index(int key)
{
    /* 坑：key 可能是负数。C 里 -5 % 13 结果是 -5（不是 8），当下标会越界。
     * 解法：先强转成 unsigned，负数变成很大的正数，取模结果一定非负。 */
    unsigned int positive_key = (unsigned int)key;
    int index = (int)(positive_key % TABLE_SIZE);
    return index;
}

/* 往哈希表里存一对 (key, value)，线性探测解决冲突 */
static void hash_put(Slot table[], int key, int value)
{
    int index = hash_index(key);
    int probe = 0;

    while (table[index].used == 1) {
        index = (index + 1) % TABLE_SIZE;   /* % 保证绕回 0，不越界 */
        probe = probe + 1;
        if (probe >= TABLE_SIZE) {
            return;   /* 表满了，放弃本次插入 */
        }
    }
    table[index].key = key;
    table[index].value = value;
    table[index].used = 1;
}

/* 查 key：找到返回对应下标，没找到返回 -1 */
static int hash_get(Slot table[], int key)
{
    int index = hash_index(key);
    int probe = 0;

    while (table[index].used == 1) {
        if (table[index].key == key) {
            return table[index].value;
        }
        index = (index + 1) % TABLE_SIZE;
        probe = probe + 1;
        if (probe >= TABLE_SIZE) {
            break;
        }
    }
    return -1;
}

int* twoSum(int* nums, int numsSize, int target, int* returnSize)
{
    Slot table[TABLE_SIZE];
    int i;

    for (i = 0; i < TABLE_SIZE; i++) {   /* 清空哈希表 */
        table[i].used = 0;
        table[i].key = 0;
        table[i].value = 0;
    }

    for (i = 0; i < numsSize; i++) {
        int need = target - nums[i];
        int found = hash_get(table, need);

        if (found != -1) {
            int* result = (int*)malloc(2 * sizeof(int));
            if (result == NULL) {
                *returnSize = 0;
                return NULL;   /* malloc 一定要判空 */
            }
            result[0] = found;
            result[1] = i;
            *returnSize = 2;
            return result;
        }
        /* 先查后插：这一行必须在 if 后面 */
        hash_put(table, nums[i], i);
    }
    *returnSize = 0;
    return NULL;
}

/* ---------- 本地测试 ---------- */
static void run_case(int nums[], int size, int target, int expect_a, int expect_b)
{
    int return_size = 0;
    int* answer = twoSum(nums, size, target, &return_size);
    int pass = 0;

    if (answer != NULL && return_size == 2) {
        if (answer[0] == expect_a && answer[1] == expect_b) {
            pass = 1;
        }
    }
    printf("target=%d -> ", target);
    if (answer != NULL) {
        printf("[%d,%d] ", answer[0], answer[1]);
    } else {
        printf("NULL ");
    }
    printf("expect [%d,%d] => %s\n", expect_a, expect_b, pass ? "PASS" : "FAIL");
    free(answer);
}

int main(void)
{
    int case1[] = {2, 7, 11, 15};
    int case2[] = {3, 2, 4};
    int case3[] = {3, 3};
    int case4[] = {-1, -2, -3, -4, -5};
    int case5[] = {-3, 4, 3, 90};

    printf("==== LeetCode 1 Two Sum (hash version) ====\n");
    run_case(case1, 4, 9, 0, 1);      /* 2 + 7 = 9 */
    run_case(case2, 3, 6, 1, 2);      /* 2 + 4 = 6，不能返回 [0,0] */
    run_case(case3, 2, 6, 0, 1);      /* 3 + 3 = 6 */
    run_case(case4, 5, -8, 2, 4);     /* -3 + -5 = -8，负数取模考点 */
    run_case(case5, 4, 0, 0, 2);      /* -3 + 3 = 0 */
    return 0;
}
