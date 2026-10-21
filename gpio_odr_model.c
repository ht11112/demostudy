/*
 * 数字电路第 4 讲 应用篇：用 16 位寄存器模型模拟 STM32 的 GPIO ODR / BSRR
 * ------------------------------------------------------------------
 * 本课要串起来的一条线：
 *   第 4 讲学过"寄存器 = n 个 D 触发器并排，共享时钟，并行加载"
 *   STM32 的 GPIOB->ODR 就是这样一个 16 位寄存器（16 个触发器并排）
 *   GPIOB->BSRR 是另一个 16 位寄存器，但它不是"存储"，是"动作触发"
 *
 * 关键对照（背下来，阶段 2 天天用）：
 *   ODR   (offset 0x0C) : 输出数据寄存器。读它得到 16 个引脚的当前电平；
 *                         写它 = 整 16 位一次性并行加载（本文件就是模拟这个）
 *   BSRR  (offset 0x10) : 低 16 位 = 置位（写 1 的位变高），
 *                         高 16 位 = 复位（写 1 的位变低）。写 0 = 这一位不动
 *   -> BSRR 是"原子操作"：一次总线写完成，不会打断别的位
 *   -> 为什么不用 ODR = ODR | (1<<5) ？那是"读-改-写"三步，
 *      中间若来了中断改了别的引脚，那一位就被抢回去了（竞态）
 *
 * 编译：gcc -Wall -Wextra -O2 gpio_odr_model.c -o gpio_odr_model.exe
 */

#include <stdio.h>

/* 16 位掩码：只保留低 16 位（GPIO 一组 16 个引脚） */
#define MASK16 0xFFFFu

/* ------------------------------------------------------------------
 * GPIO 端口模型
 * 用两个字段分别记录"引脚电平"和"引脚方向"，
 * 与真实硬件 GPIOx->ODR / GPIOx->MODER 一一对应
 * ------------------------------------------------------------------ */
typedef struct {
    unsigned int odr;   /* 输出数据寄存器，低 16 位有效 */
    unsigned int moder; /* 方向寄存器，1 = 输出，0 = 输入（简化版） */
} gpio_port_t;

/* ------------------------------------------------------------------
 * 操作 1：BSRR 置位 / 复位（模拟真实硬件的原子写）
 *   set_bits   : 低 16 位，写 1 的位置 1
 *   reset_bits : 高 16 位，写 1 的位置 0
 * 两者同时出现时，硬件行为是"置位优先"
 * ------------------------------------------------------------------ */
void gpio_write_bsrr(gpio_port_t *port, unsigned int value)
{
    unsigned int set_bits;
    unsigned int reset_bits;
    unsigned int new_level;

    /* 取值：只看低 16 位（高 16 位是复位控制位，单独取） */
    set_bits = value & 0x0000FFFFu;
    reset_bits = (value >> 16) & 0x0000FFFFu;

    /* 关键顺序：先复位，再置位
     * 这样同一位既置位又复位时，最终是"置位"生效 —— 与 STM32 手册一致 */
    new_level = port->odr & MASK16;
    new_level = new_level & (~reset_bits & MASK16);
    new_level = new_level | set_bits;

    port->odr = new_level & MASK16;
}

/* ------------------------------------------------------------------
 * 操作 2：整 16 位并行加载（模拟 GPIOx->ODR = 0x0020 这一行）
 * 对应数字电路里的"寄存器带并行加载端"
 * ------------------------------------------------------------------ */
void gpio_write_odr(gpio_port_t *port, unsigned int value)
{
    port->odr = value & MASK16;
}

/* ------------------------------------------------------------------
 * 操作 3：读某一位电平（模拟 (GPIOB->ODR & (1<<5)) ? 1 : 0）
 * ------------------------------------------------------------------ */
int gpio_read_pin(const gpio_port_t *port, int pin)
{
    unsigned int mask;
    int level;

    mask = 1u << pin;
    if ((port->odr & mask) != 0) {
        level = 1;
    } else {
        level = 0;
    }
    return level;
}

/* ------------------------------------------------------------------
 * 操作 4：把一个 16 位值按二进制打印出来（从 bit15 打到 bit0）
 * 这样一眼就能看出"哪个引脚亮了"
 * ------------------------------------------------------------------ */
void gpio_print(const char *tag, const gpio_port_t *port)
{
    int bit;

    printf("%-22s 0x%04X  ", tag, port->odr);
    printf("b");
    for (bit = 15; bit >= 0; bit--) {
        if ((port->odr & (1u << bit)) != 0) {
            printf("1");
        } else {
            printf("0");
        }
        /* 每 4 位插一个点，方便数清楚是 bit3 还是 bit4 */
        if ((bit % 4 == 0) && (bit > 0)) {
            printf(".");
        }
    }
    printf("\n");
}

/* ------------------------------------------------------------------
 * 用例 1：并行加载 + 位操作 BSRR
 * 验证点：
 *   写 ODR 后 16 位整体就是新值（并行加载，不是一位一位移进去）
 *   BSRR 置 bit5 / 复位 bit0，其他位一个都不许变
 * ------------------------------------------------------------------ */
static void test_bsrr(void)
{
    gpio_port_t port;
    int pass;

    printf("=== test 1: ODR parallel load + BSRR atomic set/reset ===\n");

    port.odr = 0x0000;
    port.moder = 0xFFFF;

    gpio_write_odr(&port, 0x0020);            /* 只点亮 bit5 */
    gpio_print("after ODR = 0x0020", &port);
    if (port.odr == 0x0020) {
        printf("  parallel load:      PASS\n");
    } else {
        printf("  parallel load:      FAIL (got 0x%04X)\n", port.odr);
    }

    /* 一次性：置 bit3 + bit9，同时复位 bit5 —— 一条指令办三件事 */
    gpio_write_bsrr(&port, 0x00000208u | (0x0020u << 16));
    gpio_print("after BSRR set3,9 rst5", &port);
    pass = (port.odr == 0x0208u);
    if (pass) {
        printf("  BSRR multi-bit:     PASS\n");
    } else {
        printf("  BSRR multi-bit:     FAIL (got 0x%04X, want 0x0208)\n", port.odr);
    }

    /* 同一位既置位又复位 -> 置位优先 */
    gpio_write_bsrr(&port, 0x00000008u | (0x00000008u << 16));
    gpio_print("BSRR set3 and rst3", &port);
    if ((port.odr & 0x0008u) != 0) {
        printf("  set beats reset:    PASS\n");
    } else {
        printf("  set beats reset:    FAIL\n");
    }
}

/* ------------------------------------------------------------------
 * 用例 2：读-改-写 vs BSRR 的竞态
 * 模拟：主循环想点亮 bit0，但执行到一半被"中断"打断，
 *       中断里用 ODR 读写把 bit7 关掉了，主循环回写时把旧值一起写回
 * 结论：BSRR 不参与"读旧值"，所以不会被抢回去
 * ------------------------------------------------------------------ */
static void test_race(void)
{
    gpio_port_t port_a;
    gpio_port_t port_b;
    unsigned int temp;

    printf("\n=== test 2: read-modify-write race vs BSRR ===\n");

    /* ---- 写法 A：ODR = ODR | mask（三步：读、改、写） ---- */
    port_a.odr = 0x0080;   /* bit7 亮着，主循环想再点亮 bit0 */

    temp = port_a.odr;                 /* 第 1 步：读到 0x0080 */
    /* 此刻被中断打断：中断里执行了 gpio_write_odr(&port_a, 0x0000); */
    gpio_write_odr(&port_a, 0x0000);   /* 中断把 bit7 关了 */
    temp = temp | 0x0001u;             /* 第 2 步：改 */
    gpio_write_odr(&port_a, temp);     /* 第 3 步：写回 0x0081 */
    gpio_print("A: RMW via ODR", &port_a);
    if (port_a.odr == 0x0081u) {
        printf("  result: bit7 came back -> LOST UPDATE\n");
    } else {
        printf("  result: unexpected 0x%04X\n", port_a.odr);
    }

    /* ---- 写法 B：BSRR（一步原子写，不需要读旧值） ---- */
    port_b.odr = 0x0080;
    gpio_write_odr(&port_b, 0x0000);              /* 同样的中断 */
    gpio_write_bsrr(&port_b, 0x00000001u);        /* 主循环只写 BSRR */
    gpio_print("B: same flow via BSRR", &port_b);
    if (port_b.odr == 0x0001u) {
        printf("  result: bit7 stays OFF    -> SAFE (no lost update)\n");
    } else {
        printf("  result: unexpected 0x%04X\n", port_b.odr);
    }

    printf("  read this: BSRR is atomic; ODR |= mask is read-modify-write\n");
    printf("             on MCU an interrupt can sneak in between the 3 steps\n");
}

/* ------------------------------------------------------------------
 * 用例 3：把所有 16 个引脚依次点亮（经典"流水灯"位运算底子）
 * 只用位移 + 掩码，不写 16 个 if
 * ------------------------------------------------------------------ */
static void test_walk(void)
{
    gpio_port_t port;
    int step;

    printf("\n=== test 3: walk one bit across 16 pins ===\n");

    port.odr = 0x0000;
    port.moder = 0xFFFF;

    for (step = 0; step < 16; step++) {
        gpio_write_odr(&port, 1u << step);
        if (step % 4 == 0) {
            printf("  step %2d: ", step);
            gpio_print("", &port);
        }
    }
    printf("  read this: (1u << pin) is the only mask you need\n");
    printf("             use 1u, not 1 -> pin 15 with signed int is still ok,\n");
    printf("             but pin 31 on a 32-bit port would break\n");
}

int main(void)
{
    /* 关缓冲：万一崩，崩之前的输出不丢 */
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("=== STM32 GPIO ODR / BSRR register model ===\n");
    printf("build: gcc -Wall -Wextra -O2 gpio_odr_model.c -o gpio_odr_model.exe\n\n");

    test_bsrr();
    test_race();
    test_walk();

    printf("\n=== done ===\n");
    return 0;
}
