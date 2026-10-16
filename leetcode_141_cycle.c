/*
 * leetcode_141_cycle.c —— 力扣 141 环形链表（快慢指针 / Floyd 判圈）
 *
 * 题目：给一个链表头，判断链表中是否有环。
 *
 * 核心比喻：
 *   环形跑道上两个人，一个跑得快（每步 2 格）、一个跑得慢（每步 1 格）。
 *   如果跑道是环形的，快的人迟早会从后面追上慢的人（两人相遇）。
 *   如果不是环形（有终点），快的人先跑到终点，永远不会相遇。
 *
 * 复杂度：时间 O(n)，空间 O(1)（只用了两个指针，不开哈希表）
 *
 * 编译（Windows + MinGW）：
 *   gcc -Wall -Wextra -O2 leetcode_141_cycle.c -o lc141.exe
 */

#include <stdio.h>
#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

/* ---------------- 力扣提交部分 ---------------- */

int hasCycle(struct ListNode *head)
{
    struct ListNode *slow = head;
    struct ListNode *fast = head;

    /*
     * 两个条件都不能少：
     *   fast != NULL        防止 fast->next->next 解引用空指针
     *   fast->next != NULL  同上（快指针每次要跨两步，先看落脚点存不存在）
     */
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;              /* 慢指针：每次走一步 */
        fast = fast->next->next;        /* 快指针：每次走两步 */

        if (slow == fast) {             /* 追上了 = 有环 */
            return 1;
        }
    }

    return 0;                           /* 快指针走到链尾 = 无环 */
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

static void check(const char *name, int ok)
{
    printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
}

int main(void)
{
    int pass = 1;

    printf("=== lc141 linked list cycle ===\n");

    /* 用例 1：无环，3 -> 2 -> 0 -> -4，期望 0 */
    {
        struct ListNode *a = node_new(3);
        struct ListNode *b = node_new(2);
        struct ListNode *c = node_new(0);
        struct ListNode *d = node_new(-4);
        int result = 0;

        a->next = b;
        b->next = c;
        c->next = d;
        /* d->next 保持 NULL —— 无环 */

        result = hasCycle(a);
        check("case1 no cycle expects 0", result == 0);
        if (result != 0) {
            pass = 0;
        }
    }

    /* 用例 2：有环，3 -> 2 -> 0 -> -4 -> 回到 2，期望 1 */
    {
        struct ListNode *a = node_new(3);
        struct ListNode *b = node_new(2);
        struct ListNode *c = node_new(0);
        struct ListNode *d = node_new(-4);
        int result = 0;

        a->next = b;
        b->next = c;
        c->next = d;
        d->next = b;            /* 手工造环：尾结点指回第二个结点 */

        result = hasCycle(a);
        check("case2 cycle to second node expects 1", result == 1);
        if (result != 1) {
            pass = 0;
        }
    }

    /* 用例 3：单结点自环，1 -> 自己，期望 1 */
    {
        struct ListNode *a = node_new(1);
        int result = 0;

        a->next = a;            /* 自己指自己，最容易漏判的边界（先移动再比较才抓得住） */

        result = hasCycle(a);
        check("case3 self cycle expects 1", result == 1);
        if (result != 1) {
            pass = 0;
        }
    }

    /* 用例 4：单结点无环，1 -> NULL，期望 0 */
    {
        struct ListNode *a = node_new(1);
        int result = 0;

        result = hasCycle(a);
        check("case4 single node no cycle expects 0", result == 0);
        if (result != 0) {
            pass = 0;
        }
    }

    /* 用例 5：空链表，期望 0（while 条件当场为假，不崩） */
    {
        int result = hasCycle(NULL);
        check("case5 null head expects 0", result == 0);
        if (result != 0) {
            pass = 0;
        }
    }

    printf("=== %s ===\n", pass ? "ALL PASS" : "SOME FAILED");
    return pass ? 0 : 1;

    /*
     * 说明：上面的用例 2/3 造了环，故意没有 free —— 带环链表无法用普通
     * 递归/循环 free，会死循环。测试程序退出即释放，属于可接受的做法。
     */
}
