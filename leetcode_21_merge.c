/*
 * leetcode_21_merge.c —— 力扣 21 合并两个有序链表
 *
 * 题目：把两个升序链表合并成一个升序链表，要求拼原有的结点（不新建结点）。
 *
 * 灵魂技巧：sentinel（哨兵 / 虚拟头结点）
 *   如果不用哨兵，就要写一堆"结果链表第一个结点该挂谁"的特判代码。
 *   放一个假头结点在前面，所有结点都统一挂到 tail->next 上，
 *   最后返回 dummy.next（跳过假头）即可 —— 代码短一半，也少一半 bug。
 *
 * 这也是「链表版归并」的骨架，归并排序的 merge 步骤就是这个函数。
 *
 * 编译（Windows + MinGW）：
 *   gcc -Wall -Wextra -O2 leetcode_21_merge.c -o lc21.exe
 */

#include <stdio.h>
#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

/* ---------------- 力扣提交部分 ---------------- */

struct ListNode *mergeTwoLists(struct ListNode *a, struct ListNode *b)
{
    struct ListNode dummy;          /* 栈上的假头结点，不占堆、无需 free */
    struct ListNode *tail = &dummy; /* tail 始终指向结果链表的最后一个结点 */

    dummy.next = NULL;

    while (a != NULL && b != NULL) {
        if (a->val <= b->val) {     /* 用 <= 而不是 < ：保证相等元素保持原相对次序（稳定） */
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;          /* 关键：每挂一个结点，tail 必须往前推进 */
    }

    /* 循环结束时必有一条链还剩结点，整串挂上去即可（已有序，无需再比） */
    if (a != NULL) {
        tail->next = a;
    } else {
        tail->next = b;
    }

    return dummy.next;              /* 返回真头：跳过假头 */
}

/* ---------------- 本地测试脚手架 ---------------- */

static struct ListNode *node_new(int value)
{
    struct ListNode *n = (struct ListNode *)malloc(sizeof(struct ListNode));
    if (n == NULL) {
        printf("[ERROR] malloc failed\n");
        exit(1);
    }
    n->val = value;
    n->next = NULL;
    return n;
}

static struct ListNode *build(int *values, int count)
{
    struct ListNode *head = NULL;
    struct ListNode *tail = NULL;
    int i = 0;

    for (i = 0; i < count; i++) {
        struct ListNode *n = node_new(values[i]);
        if (head == NULL) {
            head = n;
            tail = n;
        } else {
            tail->next = n;
            tail = n;
        }
    }
    return head;
}

static void free_list(struct ListNode *head)
{
    struct ListNode *cur = head;
    struct ListNode *next = NULL;

    while (cur != NULL) {
        next = cur->next;
        free(cur);
        cur = next;
    }
}

/* 打印形如 [1,1,2,3,4,4]；返回实际结点个数 */
static int print_and_count(struct ListNode *head)
{
    struct ListNode *cur = head;
    int count = 0;

    printf("[");
    while (cur != NULL) {
        if (count > 0) {
            printf(",");
        }
        printf("%d", cur->val);
        count++;
        cur = cur->next;
    }
    printf("]");
    return count;
}

/* 校验链表是否严格升序（非递减），并返回结点个数 */
static int is_sorted_asc(struct ListNode *head)
{
    struct ListNode *cur = head;
    int count = 0;

    while (cur != NULL) {
        if (cur->next != NULL && cur->val > cur->next->val) {
            return -1;      /* 不是升序 */
        }
        count++;
        cur = cur->next;
    }
    return count;
}

static void run_case(const char *name, int *av, int an, int *bv, int bn, int expect_count)
{
    struct ListNode *a = build(av, an);
    struct ListNode *b = build(bv, bn);
    struct ListNode *merged = NULL;
    int count = 0;
    int sorted_count = 0;
    int ok = 0;

    merged = mergeTwoLists(a, b);

    printf("case %s: ", name);
    count = print_and_count(merged);

    sorted_count = is_sorted_asc(merged);
    ok = (count == expect_count) && (sorted_count == expect_count);
    printf("  -> %s (count=%d)\n", ok ? "PASS" : "FAIL", count);

    free_list(merged);
}

int main(void)
{
    printf("=== lc21 merge two sorted lists ===\n");

    /* 用例 1：经典 [1,2,4] + [1,3,4] -> [1,1,2,3,4,4] */
    {
        int av[3] = {1, 2, 4};
        int bv[3] = {1, 3, 4};
        run_case("1", av, 3, bv, 3, 6);
    }

    /* 用例 2：空 + [1] -> [1] */
    {
        int bv[1] = {1};
        run_case("2", NULL, 0, bv, 1, 1);
    }

    /* 用例 3：[5] + 空 -> [5] */
    {
        int av[1] = {5};
        run_case("3", av, 1, NULL, 0, 1);
    }

    /* 用例 4：两边都空 -> [] */
    {
        run_case("4", NULL, 0, NULL, 0, 0);
    }

    /* 用例 5：一边全小于另一边 [1,2,3] + [8,9] -> [1,2,3,8,9] */
    {
        int av[3] = {1, 2, 3};
        int bv[2] = {8, 9};
        run_case("5", av, 3, bv, 2, 5);
    }

    printf("=== ALL CASES DONE ===\n");
    return 0;
}
