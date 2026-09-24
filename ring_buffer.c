/*
 * ring_buffer.c  —— 阶段 1 官方验收项：环形缓冲区（Ring Buffer）
 *
 * 为什么嵌入式必须会写环形缓冲区？
 *   串口接收数据是"随时来的"，CPU 处理速度和处理时机跟接收不同步。
 *   环形缓冲区就是"接收方往里放、处理方往外取"的队列，
 *   缓冲区满了从头覆盖（或者丢新数据），永远不会越界 —— 这是它最大的价值。
 *
 * 本文件实现一个经典的"头尾指针 + 计数"版本：
 *   head : 下一个要写入的位置（生产者动它）
 *   tail : 下一个要读出的位置（消费者动它）
 *   count: 当前已存元素个数（用它来区分"满"和"空"）
 *
 * 为什么需要 count？
 *   只靠 head == tail 判断，满和空的条件一模一样，无法区分。
 *   所以要么浪费一个格子，要么加一个 count —— 这里用 count，更好懂。
 *
 * 编译命令：
 *   gcc -Wall -Wextra -O2 ring_buffer.c -o ring_buffer
 *   ./ring_buffer
 */

#include <stdio.h>
#include <string.h>   /* memset */

#define RB_SIZE 8   /* 缓冲区容量，故意取小一点方便演示"满"的情况 */

typedef struct {
    unsigned char buf[RB_SIZE];   /* 真正存数据的地方 */
    int head;                     /* 写指针 */
    int tail;                     /* 读指针 */
    int count;                    /* 当前元素个数 */
} RingBuffer;

/* ============================================================
 * 初始化：把三个下标清零
 * 注意：嵌入式里这个函数要能被"上电只调一次"地安全调用
 * ============================================================ */
void rb_init(RingBuffer *rb)
{
    memset(rb->buf, 0, sizeof(rb->buf));   /* 清数据，方便调试时看脏值 */
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* 判满：个数已经等于容量 */
int rb_is_full(const RingBuffer *rb)
{
    return rb->count == RB_SIZE;
}

/* 判空：一个元素都没有 */
int rb_is_empty(const RingBuffer *rb)
{
    return rb->count == 0;
}

/* 当前元素个数 */
int rb_count(const RingBuffer *rb)
{
    return rb->count;
}

/* ============================================================
 * 写入一个字节
 * 返回 1 = 成功，0 = 缓冲区已满，写入失败
 *
 * 为什么返回状态而不是直接覆盖？
 *   嵌入式里"丢数据"是严重问题，必须让调用者知道。
 *   串口中断里通常记录一个 overflow 计数，方便事后定位问题。
 * ============================================================ */
int rb_put(RingBuffer *rb, unsigned char data)
{
    if (rb_is_full(rb)) {
        return 0;   /* 满了，拒绝写入（不覆盖旧数据） */
    }

    rb->buf[rb->head] = data;
    rb->head = (rb->head + 1) % RB_SIZE;   /* 走到末尾就绕回 0，这就是"环形" */
    rb->count++;
    return 1;
}

/* ============================================================
 * 读出一个字节
 * 返回 1 = 成功（*data 里是读出的值），0 = 缓冲区为空
 *
 * 为什么要传指针 *data？
 *   C 语言函数只能 return 一个值，这里既要返回"成功/失败"，
 *   又要带回"读到的数据"，所以数据用指针参数带出来。
 * ============================================================ */
int rb_get(RingBuffer *rb, unsigned char *data)
{
    if (rb_is_empty(rb)) {
        return 0;
    }

    *data = rb->buf[rb->tail];
    rb->tail = (rb->tail + 1) % RB_SIZE;
    rb->count--;
    return 1;
}

/* ============================================================
 * 查看下一个要读的字节，但不移动 tail（"偷看"）
 * 返回 1 = 成功，0 = 空
 * ============================================================ */
int rb_peek(const RingBuffer *rb, unsigned char *data)
{
    if (rb_is_empty(rb)) {
        return 0;
    }

    *data = rb->buf[rb->tail];
    return 1;
}

/* ============================================================
 * 清空缓冲区（只重置下标，不擦数据）
 * ============================================================ */
void rb_flush(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* 打印内部状态，调试用 */
void rb_dump(const RingBuffer *rb, const char *tag)
{
    int i;

    printf("%-16s head=%d tail=%d count=%d/%d  buf=[", tag,
           rb->head, rb->tail, rb->count, RB_SIZE);

    for (i = 0; i < RB_SIZE; i++) {
        printf("%02X", rb->buf[i]);
        if (i != RB_SIZE - 1) printf(" ");
    }
    printf("]  %s\n", rb_is_empty(rb) ? "EMPTY" : (rb_is_full(rb) ? "FULL" : ""));
}

int main(void)
{
    RingBuffer rb;
    unsigned char v;
    int i;
    int ok;

    rb_init(&rb);
    printf("=== 1) init ===\n");
    rb_dump(&rb, "after init");

    /* ---------- 测试 1：写入 5 个字节 ---------- */
    printf("\n=== 2) put 5 bytes: A1 A2 A3 A4 A5 ===\n");
    for (i = 1; i <= 5; i++) {
        ok = rb_put(&rb, (unsigned char)(0xA0 + i));
        printf("put 0x%02X -> %s  (count=%d)\n", 0xA0 + i, ok ? "OK" : "FULL", rb.count);
    }
    rb_dump(&rb, "after 5 puts");

    /* ---------- 测试 2：peek 不消费 ---------- */
    printf("\n=== 3) peek then get ===\n");
    if (rb_peek(&rb, &v)) {
        printf("peek -> 0x%02X (tail should NOT move, count still %d)\n", v, rb.count);
    }
    if (rb_get(&rb, &v)) {
        printf("get  -> 0x%02X (tail moved, count=%d)\n", v, rb.count);
    }

    /* ---------- 测试 3：写满，验证拒绝写入 ---------- */
    printf("\n=== 4) fill it up, then try one more ===\n");
    while (!rb_is_full(&rb)) {
        ok = rb_put(&rb, 0xEE);
        printf("put 0xEE -> %s (count=%d)\n", ok ? "OK" : "FULL", rb.count);
    }
    rb_dump(&rb, "when full");

    ok = rb_put(&rb, 0x99);   /* 必须失败 */
    printf("put 0x99 when FULL -> %s (expect FULL/reject)\n", ok ? "OK" : "FULL");
    printf("check: %s\n", (ok == 0) ? "PASS" : "FAIL");

    /* ---------- 测试 4：读空，验证返回失败 ---------- */
    printf("\n=== 5) drain it, then try one more ===\n");
    i = 0;
    while (rb_get(&rb, &v)) {
        i++;
    }
    printf("drained %d bytes, count=%d\n", i, rb.count);

    ok = rb_get(&rb, &v);   /* 必须失败 */
    printf("get when EMPTY -> %s (expect fail)\n", ok ? "OK" : "EMPTY/fail");
    printf("check: %s\n", (ok == 0) ? "PASS" : "FAIL");

    /* ---------- 测试 5：绕圈（核心！验证 % RB_SIZE 真的在绕） ---------- */
    printf("\n=== 6) wrap-around test (the real point of a ring buffer) ===\n");
    rb_flush(&rb);
    rb_dump(&rb, "after flush");

    /* 先写 6 个，再读 6 个 —— 此时 head 和 tail 都绕回到 6 了 */
    for (i = 0; i < 6; i++) {
        rb_put(&rb, (unsigned char)(0x10 + i));
    }
    printf("put 6 bytes (0x10..0x15), head now = %d\n", rb.head);
    for (i = 0; i < 6; i++) {
        rb_get(&rb, &v);
    }
    printf("get 6 bytes, tail now = %d  <-- 都绕回来了\n", rb.tail);

    /* 再写 5 个，这次会跨过数组末尾绕回开头 */
    for (i = 0; i < 5; i++) {
        rb_put(&rb, (unsigned char)(0x20 + i));
    }
    rb_dump(&rb, "wrapped writes");

    printf("read back: ");
    while (rb_get(&rb, &v)) {
        printf("0x%02X ", v);
    }
    printf("(expect 0x20 0x21 0x22 0x23 0x24 -- order preserved after wrap)\n");

    /* ---------- 测试 6：模拟"生产者/消费者"交替 ---------- */
    printf("\n=== 7) producer / consumer interleave ===\n");
    rb_flush(&rb);
    {
        int produced = 0;
        int consumed = 0;
        int step;

        for (step = 0; step < 20; step++) {
            /* 生产者：每步尽量放 2 个 */
            if (rb_put(&rb, (unsigned char)(produced & 0xFF))) produced++;
            if (rb_put(&rb, (unsigned char)(produced & 0xFF))) produced++;

            /* 消费者：每步取 1 个 */
            if (rb_get(&rb, &v)) {
                consumed++;
                if (consumed <= 8 || consumed > 30) {
                    printf("step %2d: got 0x%02X  (left=%d)\n", step, v, rb.count);
                }
            }
        }
        printf("total produced=%d consumed=%d left=%d\n", produced, consumed, rb.count);
        printf("check: produced - consumed == left ? %s\n",
               (produced - consumed == rb.count) ? "PASS" : "FAIL");
    }

    return 0;
}
