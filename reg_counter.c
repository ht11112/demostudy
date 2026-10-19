/*
 * 数字电路第 4 讲：寄存器与计数器（时序逻辑的"存储+计数"两件套）
 * ------------------------------------------------------------------
 * 本课目标：把前三讲的碎片连成一条线
 *   第 1 讲 与或非门            -> 组合逻辑，无记忆
 *   第 2 讲 半加器/全加器        -> 组合逻辑做运算
 *   第 3 讲 D 锁存器 / D 触发器  -> 时序逻辑，1 比特记忆
 *   第 4 讲 寄存器 / 计数器      -> 多个 D 触发器并排 = 一个 n 位寄存器
 *
 * 关键认识（本课最重要的一句话）：
 *   寄存器 = n 个 D 触发器并排放，共享同一个时钟，各自存 1 位
 *   计数器 = 寄存器 + 加法器，把"加 1 的结果"反馈回 D 端
 *
 * 与 STM32 的对应关系：
 *   STM32 的 GPIOx->ODR 就是一个 16 位寄存器（16 个触发器并排）
 *   你写 GPIOB->ODR = 0x0020; 这一行，本质是一次"并行加载"
 *   定时器的 CNT 计数器 = 本文件的 reg4_t（只是位宽大得多）
 *
 * 编译：gcc -Wall -Wextra -O2 reg_counter.c -o reg_counter.exe
 */

#include <stdio.h>

/* 4 位掩码：只保留低 4 位，模拟"只有 4 个触发器" */
#define MASK4 0x0F

/* 4 位寄存器：用结构体包装，和 STM32 的 "外设寄存器" 写法对齐 */
typedef struct {
    unsigned char bits;   /* 只有低 4 位有效，高位恒为 0 */
} reg4_t;

/* ------------------------------------------------------------------
 * 函数 1：4 位加 1（带自然溢出回绕）
 * 模拟硬件：4 位加法器 + 反馈回 D 端
 * 0xF + 1 = 0x10，截断到 4 位就是 0x0 —— 这就是"计满归零"
 * ------------------------------------------------------------------ */
unsigned char add1_4bit(unsigned char a)
{
    return (unsigned char)((a + 1) & MASK4);
}

/* ------------------------------------------------------------------
 * 函数 2：计数器走一步（一个时钟上升沿）
 * 参数 en：使能信号，EN=0 时钟来了也不动（STM32 TIM 的 CEN 位）
 * 参数 tc：终端计数输出（Terminal Count），计满 0xF 时输出 1
 *          这就是 STM32 定时器的"更新事件 UEV"，可用来触发中断
 * ------------------------------------------------------------------ */
void counter_step(reg4_t *c, unsigned char en, unsigned char *tc)
{
    if (en) {
        c->bits = add1_4bit(c->bits);
    }
    if (c->bits == MASK4) {
        *tc = 1;
    } else {
        *tc = 0;
    }
}

/* ------------------------------------------------------------------
 * 函数 3：D 触发器打入 1 位（最低位）
 * 体现"边沿敏感"：只有调用这个函数时（= 时钟上升沿）才改变状态
 * ------------------------------------------------------------------ */
void reg_clk_edge(reg4_t *r, unsigned char d)
{
    if (d != 0) {
        r->bits = (unsigned char)(r->bits | 1);
    } else {
        r->bits = (unsigned char)(r->bits & (unsigned char)~1);
    }
}

int main(void)
{
    reg4_t reg;
    int i;
    unsigned char tc;
    int pulses;
    int expect_pulses;
    int fail;

    /* ============ A. 4 位寄存器：并行加载 ============ */
    printf("=== A. 4-bit D register: shift in from LSB ===\n");
    reg.bits = 0xA;                       /* 相当于 GPIOB->ODR = 0x000A */
    printf("load 1010 -> %X (expect A)\n", reg.bits);
    reg_clk_edge(&reg, 1);                /* 上升沿打 1 进最低位，0xA -> 0xB */
    printf("clk+ D=1  -> %X (expect B)\n", reg.bits);
    reg_clk_edge(&reg, 0);                /* 上升沿打 0 进最低位，0xB -> 0xA */
    printf("clk+ D=0  -> %X (expect A)\n", reg.bits);

    /* ============ B. 4 位二进制计数器：0 -> F -> 0 ============ */
    printf("\n=== B. 4-bit binary counter (enable + terminal count) ===\n");
    reg.bits = 0;
    fail = 0;
    for (i = 0; i < 20; i++) {
        int expect = (i + 1) % 16;        /* 期望值：每个时钟 +1，满 16 归零 */
        counter_step(&reg, 1, &tc);
        printf("clk %2d -> count %X  tc=%d\n", i + 1, reg.bits, tc);
        if (reg.bits != expect) {
            fail++;
        }
    }
    printf("counter sequence check: %s\n", fail == 0 ? "PASS" : "FAIL");

    /* ============ C. 当分频器用：16 个时钟 -> 1 个脉冲 ============ */
    printf("\n=== C. divide-by-16: 64 clk edges -> 4 tc pulses ===\n");
    reg.bits = 0;
    pulses = 0;
    for (i = 0; i < 64; i++) {
        counter_step(&reg, 1, &tc);
        if (tc) {
            pulses++;
        }
    }
    expect_pulses = 4;
    printf("tc pulses = %d (expect %d) -> %s\n", pulses, expect_pulses,
           pulses == expect_pulses ? "PASS" : "FAIL");

    /* ============ D. 使能为 0：时钟来了也必须冻结 ============ */
    printf("\n=== D. enable held low: counter must freeze ===\n");
    reg.bits = 3;
    for (i = 0; i < 5; i++) {
        counter_step(&reg, 0, &tc);
    }
    printf("after 5 clk with EN=0 -> %X (expect 3) -> %s\n", reg.bits,
           reg.bits == 3 ? "PASS" : "FAIL");

    return 0;
}
