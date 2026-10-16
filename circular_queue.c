/*
 * circular_queue.c —— 循环队列（整数版），环形缓冲区的"元素数组"变体
 *
 * 与 ring_buffer.c（字节缓冲）的关系：
 *   ring_buffer.c  存的是 unsigned char（字节流，用于串口收包）
 *   circular_queue 存的是 int（元素），本质同一套 head/tail 绕回思想
 *
 * 判满的两种主流写法：
 *   写法 A「牺牲一格法」（本文件）：数组开 QCAP 格，最多存 QCAP-1 个
 *                                   满的条件 = (tail + 1) % QCAP == head
 *   写法 B「count 计数法」：        额外维护 count，满 = (count == QCAP)
 *                                   容量不浪费，但多一个变量要保持一致
 *
 * 编译（Windows + MinGW）：
 *   gcc -Wall -Wextra -O2 circular_queue.c -o cq.exe
 */

#include <stdio.h>

#define QCAP 8      /* 数组物理容量 = 最大元素数 7 + 1 个空位（牺牲一格法） */

typedef struct {
    int data[QCAP];
    int head;       /* 出队位置 */
    int tail;       /* 入队位置 */
} CircQueue;

/* ---------------- 初始化 ---------------- */

void cq_init(CircQueue *q)
{
    q->head = 0;
    q->tail = 0;
}

/* ---------------- 状态查询 ---------------- */

/* 判空：head 追上 tail */
int cq_is_empty(const CircQueue *q)
{
    return q->head == q->tail;
}

/* 判满：tail 的下一格就是 head（牺牲一格法） */
int cq_is_full(const CircQueue *q)
{
    return (q->tail + 1) % QCAP == q->head;
}

/*
 * 当前元素个数。
 * 注意不能直接写 tail - head：绕回后会得到负数，
 * 必须先加一个 QCAP 再取模，保证结果非负。
 */
int cq_count(const CircQueue *q)
{
    return (q->tail - q->head + QCAP) % QCAP;
}

/* ---------------- 入队 / 出队 ---------------- */

/* 入队：成功返回 1，队列满返回 0 */
int cq_put(CircQueue *q, int value)
{
    if (cq_is_full(q)) {
        return 0;
    }
    q->data[q->tail] = value;
    q->tail = (q->tail + 1) % QCAP;
    return 1;
}

/* 出队：成功返回 1 并把值写到 *value_out，队列空返回 0 */
int cq_get(CircQueue *q, int *value_out)
{
    if (cq_is_empty(q)) {
        return 0;
    }
    *value_out = q->data[q->head];
    q->head = (q->head + 1) % QCAP;
    return 1;
}

/* ---------------- 测试 ---------------- */

static int g_all_pass = 1;

static void check(const char *name, int ok)
{
    printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) {
        g_all_pass = 0;
    }
}

int main(void)
{
    CircQueue q;
    int i = 0;
    int v = 0;
    int ok = 0;

    printf("=== circular queue test ===\n");

    /* 测试 1：空队列 get 必须失败 */
    printf("test 1: empty queue\n");
    cq_init(&q);
    ok = (cq_get(&q, &v) == 0);
    check("get on empty returns 0", ok);
    check("is_empty on fresh queue", cq_is_empty(&q) == 1);
    check("count on fresh queue is 0", cq_count(&q) == 0);

    /* 测试 2：连续 put 7 个成功，第 8 个被拒 */
    printf("test 2: fill to capacity (7 of 8 slots)\n");
    for (i = 1; i <= 7; i++) {
        ok = cq_put(&q, i * 10);
        if (!ok) {
            printf("  [FAIL] put %d rejected too early\n", i);
            g_all_pass = 0;
        }
    }
    check("is_full after 7 puts", cq_is_full(&q) == 1);
    check("count == 7", cq_count(&q) == 7);
    check("8th put rejected", cq_put(&q, 999) == 0);

    /* 测试 3：取走 3 个再 put 3 个，验证绕回后顺序不乱 */
    printf("test 3: wrap around\n");
    for (i = 0; i < 3; i++) {
        ok = cq_get(&q, &v);
        if (!ok || v != (i + 1) * 10) {
            printf("  [FAIL] pop #%d expected %d got %d\n", i + 1, (i + 1) * 10, v);
            g_all_pass = 0;
        }
    }
    /* 此时队列里是 40 50 60 70，head=3 tail=7（还没绕回，只是往后挪了） */
    check("head advanced to 3", q.head == 3);
    check("tail at 7", q.tail == 7);
    cq_put(&q, 80);         /* tail 7 -> 0，这里才是真正"绕回数组头部" */
    check("tail wrapped to 0 after put", q.tail == 0);
    cq_put(&q, 90);
    cq_put(&q, 100);
    check("count == 7 after refill", cq_count(&q) == 7);

    /* 按 FIFO 顺序取出，必须是 40 50 60 70 80 90 100 */
    {
        int expect[7] = {40, 50, 60, 70, 80, 90, 100};
        int order_ok = 1;
        for (i = 0; i < 7; i++) {
            cq_get(&q, &v);
            if (v != expect[i]) {
                printf("  [FAIL] order at %d expected %d got %d\n", i, expect[i], v);
                order_ok = 0;
            }
        }
        check("FIFO order kept across wrap", order_ok);
    }
    check("is_empty after draining", cq_is_empty(&q) == 1);
    check("get on empty again fails", cq_get(&q, &v) == 0);

    /* 测试 4：40 次 put/get 交错，验证 count 与实际取出个数守恒 */
    printf("test 4: producer / consumer conservation\n");
    cq_init(&q);
    {
        int produced = 0;
        int consumed = 0;
        int rejected = 0;
        int mismatch = 0;

        for (i = 0; i < 40; i++) {
            if (i % 3 != 2) {           /* 2/3 的轮次生产 */
                if (cq_put(&q, i)) {
                    produced++;
                } else {
                    rejected++;
                }
            }
            if (i % 4 != 3) {           /* 3/4 的轮次消费 */
                if (cq_get(&q, &v)) {
                    consumed++;
                }
            }
            /*
             * 恒等式：produced - consumed == 当前队列元素个数
             * 任何时刻不成立就说明 head/tail/取模有 bug
             */
            if (produced - consumed != cq_count(&q)) {
                mismatch++;
            }
        }
        printf("  produced=%d consumed=%d rejected=%d in-queue=%d\n",
               produced, consumed, rejected, cq_count(&q));
        check("count identity holds at every step", mismatch == 0);
        check("count equals produced - consumed at end",
              cq_count(&q) == produced - consumed);
    }

    printf("=== %s ===\n", g_all_pass ? "ALL PASS" : "SOME FAILED");
    return g_all_pass ? 0 : 1;
}
