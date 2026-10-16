/*
 * dff_latch.c —— 数字电路第 3 讲：时序逻辑（锁存器 / D 触发器 / 移位寄存器）
 *
 * ================================================================
 * 本讲要建立的核心分界（第 2 讲提过，这一讲把它彻底钉死）：
 *
 *   组合逻辑（combinational）
 *       输出 = f(当前输入)                    —— 没有任何记忆
 *       例子：与门、或门、半加器、全加器、多路选择器
 *
 *   时序逻辑（sequential）
 *       输出 = f(当前输入, 之前存下来的状态)   —— 有记忆
 *       例子：锁存器、触发器、寄存器、计数器、状态机
 *
 *   判别口诀：**它记不记得"上一次"？记得 = 时序。**
 *
 * ================================================================
 * 硬件里的关键概念：时钟（CLK）与"边沿"
 *
 *   真实的触发器是"看时钟的节拍干活"的：
 *     - 锁存器（latch）：电平敏感 —— 时钟是高电平的整段时间里，
 *       输入一变输出就跟着变（像一扇开着的门）。所谓"透明"。
 *     - 触发器（flip-flop）：边沿敏感 —— 只在时钟"跳变的那一瞬间"
 *       把输入抓进来存住，其余时间输入随便乱动都不影响输出。
 *
 *   本文件用软件模拟时，就把"时钟跳变的那一瞬间"理解成
 *   调用一次 dff_update()。函数没被调用 = 时钟没跳 = 状态绝对不变。
 *
 * ================================================================
 * 为什么嵌入式工程师必须懂这个？
 *
 *   STM32 的"输出数据寄存器 ODR"就是一个 32 位的 D 触发器阵列：
 *     - 你写 GPIOB->ODR = 0x0020 这一句，本质是"给 32 个并排的 D 触发器
 *       同时送新数据 + 打一拍时钟"，于是 32 根引脚同时更新电平；
 *     - 你读到 ODR 的当前值，读的就是触发器里存着的状态；
 *     - 这就是为什么"配置 GPIO 后再写数据"是两步 —— 前者配置触发器
 *       的方向/模式，后者才是往存储器里塞值。
 *   寄存器（register）这个词在单片机里有两层意思，都是从这儿来的。
 *
 * 编译（Windows + MinGW）：
 *   gcc -Wall -Wextra -O2 dff_latch.c -o dff.exe
 */

#include <stdio.h>

/* ================================================================
 * 一、D 锁存器（level-sensitive，电平敏感）
 *
 * 端口：
 *   d     —— 数据输入
 *   en    —— 使能（enable），相当于时钟电平
 *   q     —— 存住的状态（输出）
 *
 * 行为：
 *   en == 1 → 门开着，q 跟着 d 变（输出"透明"）
 *   en == 0 → 门关上，q 保持上一次的值（这就是"记忆"）
 * ================================================================ */
typedef struct {
    int q;      /* 存住的状态，初值 0 */
} DLatch;

void dlatch_init(DLatch *latch)
{
    latch->q = 0;
}

/* 返回新的 q，方便调用处直接用 */
int dlatch_update(DLatch *latch, int d, int en)
{
    if (en == 1) {
        latch->q = d;       /* 门开着：直接穿过去 */
    }
    /* 门关着：什么都不做 —— 这一句"不做"就是记忆的来源 */
    return latch->q;
}

/* ================================================================
 * 二、D 触发器（edge-triggered，边沿敏感）
 *
 * 端口：d（数据）、clk（时钟）、q（状态）
 *
 * 行为：只在 clk 从 0 变成 1 的那一瞬间（上升沿）采样 d。
 *
 * 软件模拟的写法要点：
 *   函数内部必须有 prev_clk 记住"上一次的时钟值"，
 *   否则无法判断"这一拍是不是上升沿"。
 *   —— 你看，连"判断边沿"这件事本身都需要一个触发器，
 *      时序逻辑就是这么自举起来的。
 * ================================================================ */
typedef struct {
    int q;          /* 存住的状态 */
    int prev_clk;   /* 上一次看到的时钟电平，用来检测上升沿 */
} DFF;

void dff_init(DFF *ff)
{
    ff->q = 0;
    ff->prev_clk = 0;
}

/*
 * 把时钟推进一格。
 * 返回 1 表示"这一拍发生了上升沿"，0 表示没有。
 * 调用者可以通过返回值知道"状态这一拍有没有可能变"。
 */
int dff_tick(DFF *ff, int d, int clk)
{
    int rising_edge = 0;

    /* 上升沿 = 上一次是 0，这一次是 1 */
    if (ff->prev_clk == 0 && clk == 1) {
        rising_edge = 1;
        ff->q = d;          /* 只在边沿这一刻抓数据 */
    }

    ff->prev_clk = clk;     /* 更新"上一次的时钟"，必须放在最后 */
    return rising_edge;
}

/* ================================================================
 * 三、移位寄存器（4 位）：4 个 D 触发器串起来
 *
 *   数据流向：  din -> [FF3] -> [FF2] -> [FF1] -> [FF0]
 *
 *   每一个时钟上升沿：
 *     FF3 抓 din，FF2 抓 FF3 的旧值，FF1 抓 FF2 的旧值，FF0 抓 FF1 的旧值。
 *
 *   初值（复位后）全部是 0。第 1 个上升沿把 din 送进 bit3：
 *     第 1 拍 value = 0b1000，第 2 拍 0b1100，第 3 拍 0b1110，第 4 拍 0b1111。
 *   即"1"这个比特从最低位一位一位被挤向最高位 —— 这就是"移位"。
 *   （注意方向：**串行输入进的是最高位，移位方向是向高位**。
 *     这和 SPI 里"先发 MSB"的习惯一致。）
 *
 *   STM32 里对应什么？SPI 移位寄存器。你往 SPI->DR 写一个字节，
 *   硬件就是靠这样一串触发器把 8 个比特一位一位地推出去的。
 * ================================================================ */
typedef struct {
    int bit0;   /* 最低位，最先到达 */
    int bit1;
    int bit2;
    int bit3;   /* 最高位，最后到达 */
} ShiftReg4;

void sreg_init(ShiftReg4 *reg)
{
    reg->bit0 = 0;
    reg->bit1 = 0;
    reg->bit2 = 0;
    reg->bit3 = 0;
}

/*
 * 打一拍。
 *
 * 这里是最容易写错的地方：必须先用 4 个临时变量把所有"旧值"抄下来，
 * 再统一写回去。
 *   如果直接写成  reg->bit1 = reg->bit0; reg->bit2 = reg->bit1;
 *   那么第二句读到的 bit1 已经是刚被覆盖过的新值了 —— 结果是
 *   "同一个值传遍 4 级"，而不是移位。这叫"非阻塞赋值"问题，
 *   在 Verilog 里用 <= 解决，在 C 里靠临时变量解决。
 */
void sreg_tick(ShiftReg4 *reg, int din)
{
    int old0 = reg->bit0;
    int old1 = reg->bit1;
    int old2 = reg->bit2;
    int old3 = reg->bit3;

    reg->bit0 = old1;       /* 低位接到上一级 */
    reg->bit1 = old2;
    reg->bit2 = old3;
    reg->bit3 = din;        /* 最高位接串行输入 */

    (void)old0;             /* old0 被"挤出"寄存器，没人再用它了 —— 相当于移出的比特 */
}

/* 把 4 位状态读成一个整数（bit3 是最高位） */
int sreg_value(const ShiftReg4 *reg)
{
    return (reg->bit3 << 3) | (reg->bit2 << 2) | (reg->bit1 << 1) | reg->bit0;
}

/* ================================================================
 * 测试脚手架
 * ================================================================ */
static int g_all_pass = 1;

static void check(const char *name, int ok)
{
    printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) {
        g_all_pass = 0;
    }
}

/* ---- 测试 1：D 锁存器的"透明"与"保持"两种状态 ---- */
static void test_latch(void)
{
    DLatch latch;

    printf("\n--- test 1: D latch (level sensitive) ---\n");
    dlatch_init(&latch);

    /* 使能开着：d 变 q 就跟着变（透明） */
    check("en=1 d=1 -> q=1", dlatch_update(&latch, 1, 1) == 1);
    check("en=1 d=0 -> q=0", dlatch_update(&latch, 0, 1) == 0);

    /* 关键：先把 q 变成 1，再关掉使能，然后乱改 d */
    dlatch_update(&latch, 1, 1);
    check("en=1 d=1 -> q=1 again", latch.q == 1);
    check("en=0 keeps q=1 (d=0)", dlatch_update(&latch, 0, 0) == 1);
    check("en=0 keeps q=1 (d=1)", dlatch_update(&latch, 1, 0) == 1);
    check("en=0 keeps q=1 (d=0 back)", dlatch_update(&latch, 0, 0) == 1);
    check("value really held (memory works)", latch.q == 1);
}

/* ---- 测试 2：D 触发器的"只在边沿抓数据" ---- */
static void test_dff(void)
{
    DFF ff;
    int edge = 0;

    printf("\n--- test 2: D flip-flop (edge triggered) ---\n");
    dff_init(&ff);

    /* clk 一直是 0，d 怎么变都没用 —— 没边沿就不采样 */
    dff_tick(&ff, 1, 0);
    check("clk=0, d=1 -> q stays 0 (no edge)", ff.q == 0);
    dff_tick(&ff, 1, 0);
    check("clk=0 again -> q still 0", ff.q == 0);

    /* 产生上升沿，此刻 d=1，于是 q 抓成 1 */
    edge = dff_tick(&ff, 1, 1);
    check("rising edge detected", edge == 1);
    check("q captured d=1 on edge", ff.q == 1);

    /* clk 保持 1 不变：d 改成 0，q 不受影响（这是触发器 vs 锁存器的分界！） */
    dff_tick(&ff, 0, 1);
    check("clk held high, d=0 -> q stays 1", ff.q == 1);

    /* 时钟下降沿：不采样 */
    dff_tick(&ff, 0, 0);
    check("falling edge -> q stays 1", ff.q == 1);

    /* 再来一个上升沿，这次 d=0，q 才变成 0 */
    dff_tick(&ff, 0, 1);
    check("next rising edge captures d=0", ff.q == 0);
}

/* ---- 测试 3：移位寄存器 ---- */
static void test_shift_reg(void)
{
    ShiftReg4 reg;
    int i = 0;
    int expect[4] = {8, 12, 14, 15};

    printf("\n--- test 3: 4-bit shift register ---\n");
    sreg_init(&reg);

    printf("  tick0: value=%d (0b%d%d%d%d)\n", sreg_value(&reg),
           reg.bit3, reg.bit2, reg.bit1, reg.bit0);
    check("initial value is 0", sreg_value(&reg) == 0);

    /* 连续送 4 个 1 进去，看是不是一位一位被推上来 */
    for (i = 0; i < 4; i++) {
        sreg_tick(&reg, 1);
        printf("  tick%d: din=1 value=%d (0b%d%d%d%d)\n", i + 1, sreg_value(&reg),
               reg.bit3, reg.bit2, reg.bit1, reg.bit0);
        if (sreg_value(&reg) != expect[i]) {
            printf("  [FAIL] tick%d expected %d got %d\n", i + 1, expect[i], sreg_value(&reg));
            g_all_pass = 0;
        }
    }
    check("shifted 1 from LSB to MSB correctly", sreg_value(&reg) == 15);

    /* 再送一个 0：最左边那个 1 被挤出寄存器，剩下 0b0111 = 7 */
    sreg_tick(&reg, 0);
    printf("  tick5: din=0 value=%d (0b%d%d%d%d)\n", sreg_value(&reg),
           reg.bit3, reg.bit2, reg.bit1, reg.bit0);
    check("one more tick with din=0 gives 7 (MSB pushed out)", sreg_value(&reg) == 7);

    /*
     * 对拍：用整数移位来验证硬件行为
     *
     *   ★ 坑点（本人本轮真实踩到）：软件对照模型必须和硬件**同向**。
     *     硬件是"新比特进 bit3（最高位），旧比特向低位挤"，
     *     所以软件模型要写 ((value >> 1) | (din << 3)) & 0xF ，
     *     而不是 ((value << 1) | din) & 0xF 。
     *     方向写反了，对拍会稳定错位 —— 这不是硬件错，是我参考模型写错。
     *     （同理：写移位寄存器测试时，先想清楚"哪端进、哪端出"。）
     */
    {
        int software = 0;
        int hw = 0;
        int feed[8] = {1, 0, 1, 1, 0, 0, 1, 1};
        int mismatch = 0;

        sreg_init(&reg);
        i = 0;
        for (i = 0; i < 8; i++) {
            sreg_tick(&reg, feed[i]);
            /* 软件模型：新比特补进最低位，旧比特向高位推 —— 与硬件同向 */
            software = ((software >> 1) | (feed[i] << 3)) & 0x0F;
            hw = sreg_value(&reg);
            if (software != hw) {
                printf("  [FAIL] step %d: sw=%d hw=%d (i=%d)\n", i + 1, software, hw, i);
                mismatch++;
            }
        }
        printf("  after 8 bits: sw=%d hw=%d\n", software, hw);
        check("8-step cross check against integer shift", mismatch == 0);
        check("only 4 bits survive (older bits shifted out)", hw == 12);
    }
}

int main(void)
{
    printf("=== digital logic lesson 3: latch / DFF / shift register ===\n");

    test_latch();
    test_dff();
    test_shift_reg();

    printf("\n=== %s ===\n", g_all_pass ? "ALL PASS" : "SOME FAILED");
    return g_all_pass ? 0 : 1;
}
