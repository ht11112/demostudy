/*
 * comb_logic.c —— 数字电路第 2 讲：组合逻辑（半加器 / 全加器真值表）
 *
 * 本讲要建立的分界（背下来，第 3 讲触发器、第 4 讲寄存器、看 STM32 原理图都靠它）：
 *
 *   组合逻辑（combinational）：输出只由"当前输入"决定，没有任何记忆。
 *       例子：与门、或门、非门、异或门、半加器、全加器、译码器、多路选择器。
 *
 *   时序逻辑（sequential）：输出还取决于"之前存下来的状态"，有记忆。
 *       例子：触发器（flip-flop）、寄存器、计数器。
 *
 *   判别口诀：**有没有"记住上一次"的能力？** 能记住 = 时序，纯算 = 组合。
 *
 * 半加器（half adder）：两个 1 位输入 A、B，算它们的和，不接收低位来的进位。
 *   和  Sum   = A ^ B     （异或门：两个输入不同才输出 1）
 *   进位 Carry = A & B     （与门：两个都是 1 才进位）
 *
 *   真值表：
 *     A B | Sum Carry
 *     0 0 |  0    0
 *     0 1 |  1    0
 *     1 0 |  1    0
 *     1 1 |  0    1        <- 1 + 1 = 10(二进制) = 进位1 和0
 *
 * 全加器（full adder）：三个输入 A、B、Cin（低位来的进位），能级联接成多位加法器。
 *   做法：先半加 A+B 得 (sum1, c1)，再把 sum1 与 Cin 半加得 (sum2, c2)，
 *         最后进位 = c1 或 c2（两次进位里任一次成立就要向上进位）。
 *
 * 编译（Windows + MinGW）：
 *   gcc -Wall -Wextra -O2 comb_logic.c -o comb.exe
 */

#include <stdio.h>

/* ---------------- 半加器 ---------------- */

/*
 * 输入：a、b（各 0 或 1）
 * 输出：函数返回值 = 和 Sum；进位通过 carry_out 指针带出去
 * 为什么用指针？C 函数的 return 只能带一个值，需要"多返回一个"就传地址。
 */
int half_adder(int a, int b, int *carry_out)
{
    *carry_out = a & b;     /* 与门：两个都是 1 才进位 */
    return a ^ b;           /* 异或门：两个不同才为 1 */
}

/* ---------------- 全加器 ---------------- */

/*
 * 输入：a、b、cin（Cin = 低位向本位送来的进位）
 * 输出：函数返回值 = 和 S；本位向高位的进位通过 cout_out 带出去
 */
int full_adder(int a, int b, int cin, int *cout_out)
{
    int c1 = 0;
    int c2 = 0;
    int sum1 = 0;
    int sum2 = 0;

    sum1 = half_adder(a, b, &c1);           /* 第一级：a + b */
    sum2 = half_adder(sum1, cin, &c2);      /* 第二级：把上一步的和再与 cin 相加 */

    *cout_out = c1 | c2;                    /* 或门：两次进位任一次成立即向上进位 */
    return sum2;
}

/* ---------------- 真值表打印 ---------------- */

static void print_half_adder_table(void)
{
    int a = 0;
    int b = 0;
    int carry = 0;
    int sum = 0;

    printf("\n--- half adder truth table ---\n");
    printf("A B | Sum Carry\n");
    printf("----|----------\n");

    for (a = 0; a <= 1; a++) {
        for (b = 0; b <= 1; b++) {
            sum = half_adder(a, b, &carry);
            printf("%d %d |  %d    %d\n", a, b, sum, carry);
        }
    }
}

static void print_full_adder_table(void)
{
    int a = 0;
    int b = 0;
    int cin = 0;
    int cout = 0;
    int sum = 0;

    printf("\n--- full adder truth table ---\n");
    printf("A B Cin | Sum Cout\n");
    printf("--------|---------\n");

    for (a = 0; a <= 1; a++) {
        for (b = 0; b <= 1; b++) {
            for (cin = 0; cin <= 1; cin++) {
                sum = full_adder(a, b, cin, &cout);
                printf("%d %d  %d  |  %d    %d\n", a, b, cin, sum, cout);
            }
        }
    }
}

/* ---------------- 用全加器做 2 位二进制加法（级联演示） ---------------- */

/*
 * 演示"级联"：低位全加器的进位输出接高位全加器的进位输入。
 * 这就是 CPU 里加法器的雏形（真实 CPU 会用超前进位等优化版）。
 *
 * 输入：两个 2 位数 a、b（0~3）
 * 输出：函数返回值 = 和的低 2 位（0~3），最高位（第 2 位）通过 carry_out 带出。
 *
 * 坑点提醒：因为结果是"3 位"（两个 2 位数相加最大 6，需要 3 个二进制位），
 * 只返回 2 位必然装不下 —— 所以最高位必须靠 carry_out 单独带出去。
 * 这也是硬件加法器的真实形态：n 位加法器输出 n 位 + 1 个进位标志（CPU 的 CF 标志位）。
 */
static int add_two_bit(int a, int b, int *carry_out)
{
    int a0 = a & 1;             /* a 的第 0 位（最低位） */
    int a1 = (a >> 1) & 1;      /* a 的第 1 位 */
    int b0 = b & 1;
    int b1 = (b >> 1) & 1;

    int c0 = 0;
    int c1 = 0;
    int s0 = 0;
    int s1 = 0;

    s0 = full_adder(a0, b0, 0, &c0);        /* 最低位：进位输入固定 0 */
    s1 = full_adder(a1, b1, c0, &c1);       /* 高位：吃低位的进位 */

    *carry_out = c1;                        /* 最高位（第 2 位）单独带出 */
    return (s1 << 1) | s0;                  /* 返回值只有低 2 位 */
}

/* 把"低 2 位返回值 + 进位"拼回一个完整的整数，用于对拍 */
static int add_two_bit_full(int a, int b)
{
    int carry = 0;
    int low = add_two_bit(a, b, &carry);

    return (carry << 2) | low;
}

int main(void)
{
    int a = 0;
    int b = 0;
    int carry = 0;
    int sum = 0;
    int mismatch = 0;

    print_half_adder_table();
    print_full_adder_table();

    /* 用整数加法对拍，验证级联加法器正确 */
    printf("\n--- 2-bit cascaded adder vs built-in + ---\n");
    printf("A B | low carry | full | A+B\n");
    printf("----|-----------|------|----\n");

    for (a = 0; a <= 3; a++) {
        for (b = 0; b <= 3; b++) {
            int full = 0;

            sum = add_two_bit(a, b, &carry);
            full = add_two_bit_full(a, b);
            printf("%d %d |  %d    %d   |  %d   | %d\n", a, b, sum, carry, full, a + b);
            if (full != a + b) {
                mismatch++;
            }
        }
    }

    printf("\nresult: %s\n", (mismatch == 0) ? "ALL PASS (adder matches integer +)" : "MISMATCH FOUND");

    return (mismatch == 0) ? 0 : 1;
}
