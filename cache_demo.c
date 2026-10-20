/*
 * 计算机组成原理第 2 讲：存储器层次结构（把"金字塔"量出来）
 * ------------------------------------------------------------------
 * 本课核心三句话：
 *   1) 存储器是金字塔：越靠上越小、越快、越贵；越靠下越大、越慢、越便宜
 *   2) CPU 只在最顶上干活，下面几层存在的意义是"让上面那层觉得自己够快"
 *   3) 局部性原理（时间局部性 + 空间局部性）是金字塔成立的全部理由
 *
 * 与嵌入式的对应（本课真正的落点，比计时更重要）：
 *   STM32F103: FLASH 64~512KB / SRAM 20~64KB / 无 Cache / 72MHz
 *   STM32F407: FLASH 1MB      / SRAM 192KB  / 有 I-Cache + D-Cache / 168MHz
 *   -> "F407 比 F103 快" 有一大半不是主频，而是多了 Cache
 *   -> 一旦有 Cache，"DMA 直接写进内存的缓冲区"就可能被你读到旧值，
 *      这就是 Cache 一致性问题 —— 阶段 2 写 OV2640/DMA 时必然撞上
 *
 * 编译（-O2 必带）：
 *   gcc -Wall -Wextra -O2 cache_demo.c -o cache_demo.exe
 * 运行：
 *   ./cache_demo.exe
 *
 * 重要提醒（本机实测结论）：
 *   现代 CPU 有硬件预取器，简单的"跨步访问"会被它自动补上，
 *   所以你在 PC 上量到的差距可能只有 2~4 倍，甚至噪声很大。
 *   这不是代码写错，而是 PC 比 MCU 聪明得多。
 *   在 STM32 上（无 Cache、主频低、SRAM 极小）这个差距是真实且致命的。
 * ------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ------------------------------------------------------------------
 * 计时工具：clock() 取 CPU 时间，Windows 上精度约 1ms
 * 所以每段实验都要跑到"几百毫秒以上"结果才可信
 * ------------------------------------------------------------------ */
static double now_seconds(void)
{
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

/* 防止编译器把整段循环优化掉：结果写进 volatile 全局变量 */
static volatile long long g_sink;

/* ------------------------------------------------------------------
 * 实验 A：顺序访问 vs 大跨度访问（空间局部性）
 * 同一块内存、同样的"读取次数"，只改每次前进的跨度
 * ------------------------------------------------------------------ */
static void exp_stride(void)
{
    const int n = 8 * 1024 * 1024;      /* 800 万个 int = 32 MB */
    const int rounds = 32;              /* 多跑几遍，让耗时够长 */
    int *buf;
    int stride;
    int r;
    int i;
    double sum;
    double t0;
    double t1;

    buf = (int *)malloc((size_t)n * sizeof(int));
    if (buf == NULL) {
        printf("[A] malloc failed\n");
        return;
    }
    for (i = 0; i < n; i++) {
        buf[i] = 1;
    }

    printf("=== A. sequential vs strided access (32 MB array, %d rounds) ===\n", rounds);
    printf("%-22s %-12s\n", "step", "time(s)");
    for (stride = 1; stride <= 64; stride = stride * 2) {
        sum = 0.0;
        t0 = now_seconds();
        for (r = 0; r < rounds; r++) {
            for (i = 0; i + stride < n; i = i + stride) {
                sum = sum + (double)buf[i];
            }
        }
        t1 = now_seconds();
        printf("step=%2d ints(%4dB)   %-12.3f\n",
               stride, stride * (int)sizeof(int), t1 - t0);
        g_sink = (long long)sum;
    }
    printf("read this: step <= 16 ints(64B) is one cache line, step >= 32 jumps lines\n");
    printf("           on a PC the prefetcher hides most of it; on MCU it does not\n\n");

    free(buf);
}

/* ------------------------------------------------------------------
 * 实验 B：行优先 vs 列优先（C 的二维数组在内存里按行连续铺开）
 * 外循环走行、内循环走列 = 地址连续 = 顺序访问
 * 外循环走列、内循环走行 = 每步跳一整行 = 最坏访问模式
 * 这个坑在刷 LCD、图像转置、矩阵乘法里必踩
 * ------------------------------------------------------------------ */
static void exp_row_col(void)
{
    const int size = 3000;              /* 3000 x 3000 int = 约 34 MB */
    const int total = size * size;
    int *mat;
    int row;
    int col;
    int k;
    double sum;
    double t0;
    double t1;

    mat = (int *)malloc((size_t)total * sizeof(int));
    if (mat == NULL) {
        printf("[B] malloc failed\n");
        return;
    }
    for (k = 0; k < total; k++) {
        mat[k] = 1;
    }

    printf("=== B. row-major vs column-major (%d x %d int) ===\n", size, size);

    /* 行优先：内层 col 变化，地址 +4 字节连续 */
    sum = 0.0;
    t0 = now_seconds();
    for (row = 0; row < size; row++) {
        for (col = 0; col < size; col++) {
            sum = sum + (double)mat[row * size + col];
        }
    }
    t1 = now_seconds();
    printf("row-major    %.3f s\n", t1 - t0);
    g_sink = (long long)sum;

    /* 列优先：内层 row 变化，地址每次 +size*4 = 12000 字节 */
    sum = 0.0;
    t0 = now_seconds();
    for (col = 0; col < size; col++) {
        for (row = 0; row < size; row++) {
            sum = sum + (double)mat[row * size + col];
        }
    }
    t1 = now_seconds();
    printf("column-major %.3f s\n", t1 - t0);
    g_sink = (long long)sum;
    printf("read this: same operation count, only the walk order changed\n");
    printf("           each step jumps %d bytes -> every step is a new cache line\n\n",
           size * (int)sizeof(int));

    free(mat);
}

/* ------------------------------------------------------------------
 * 实验 C：时间局部性 —— 反复用同一块小缓冲 vs 每次换一块新缓冲
 * 8 KB 能完整待在 L1 里；16 MB 永远待不住
 * ------------------------------------------------------------------ */
static void exp_reuse(void)
{
    const int small = 2048;                     /* 2048 int = 8 KB */
    const int rounds = 50000;
    const int big = 4 * 1024 * 1024;            /* 400 万个 int = 16 MB */
    int *hot;
    int *cold;
    int round;
    int i;
    double sum;
    double t0;
    double t1;
    int base;

    hot = (int *)malloc((size_t)small * sizeof(int));
    if (hot == NULL) {
        printf("[C] malloc failed\n");
        return;
    }
    for (i = 0; i < small; i++) {
        hot[i] = i % 7;
    }

    printf("=== C. temporal locality: reuse 8 KB vs touch cold 16 MB ===\n");

    /* 复用：同一块 8 KB，一直在 L1 里 */
    sum = 0.0;
    t0 = now_seconds();
    for (round = 0; round < rounds; round++) {
        for (i = 0; i < small; i++) {
            sum = sum + (double)hot[i];
        }
    }
    t1 = now_seconds();
    printf("reuse 8 KB buffer       %.3f s\n", t1 - t0);
    g_sink = (long long)sum;

    cold = (int *)malloc((size_t)big * sizeof(int));
    if (cold == NULL) {
        printf("[C] malloc for cold buffer failed\n");
        free(hot);
        return;
    }
    for (i = 0; i < big; i++) {
        cold[i] = i % 7;
    }

    /* 不复用：每轮摸 8 KB，但位置每轮都变，永远踩在 cache 外面 */
    sum = 0.0;
    t0 = now_seconds();
    for (round = 0; round < rounds; round++) {
        /* 关键：base 必须用 long 算
         * 若写成 int base = round * 8191，round 大时整数溢出变负数
         * -> 负下标 -> 段错误（本机调试时真踩了，务必照抄 long） */
        base = (int)(((long)round * 8191) % (big - 8192));
        for (i = 0; i < small; i++) {
            sum = sum + (double)cold[base + i];
        }
    }
    t1 = now_seconds();
    printf("touch cold 16 MB each   %.3f s\n", t1 - t0);
    g_sink = (long long)sum;
    printf("read this: the ratio is the price of a cache miss\n\n");

    free(cold);
    free(hot);
}

/* ------------------------------------------------------------------
 * 实验 D：金字塔对照表（PC vs 两个 STM32 主型号）
 * 建议手抄一遍，面试直接能讲
 * ------------------------------------------------------------------ */
static void exp_pyramid(void)
{
    printf("=== D. memory hierarchy: PC vs STM32 ===\n");
    printf("%-12s %-16s %-14s %-16s\n", "level", "PC(typical)", "STM32F103", "STM32F407");
    printf("%-12s %-16s %-14s %-16s\n", "register", "16 x 8B", "R0-R15", "R0-R15");
    printf("%-12s %-16s %-14s %-16s\n", "L1 cache", "32~64KB", "none", "0~16KB I/D");
    printf("%-12s %-16s %-14s %-16s\n", "L2 cache", "512KB~2MB", "none", "none");
    printf("%-12s %-16s %-14s %-16s\n", "L3 cache", "8~32MB", "none", "none");
    printf("%-12s %-16s %-14s %-16s\n", "SRAM/DRAM", "8~32GB", "20~64KB", "192KB");
    printf("%-12s %-16s %-14s %-16s\n", "FLASH", "256GB~1TB", "64~512KB", "1MB");
    printf("\n");
    printf("1. every level is a cache for the level below it\n");
    printf("2. smaller = faster = must be planned for earlier\n");
    printf("3. on F407, a stale DMA buffer is a bug, not a slowdown\n\n");
}

int main(void)
{
    /* 关掉输出缓冲：万一崩溃，崩溃前的输出不会丢（调试段错误的好习惯） */
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("=== memory hierarchy demo ===\n");
    printf("build: gcc -Wall -Wextra -O2 cache_demo.c -o cache_demo.exe\n\n");

    exp_pyramid();
    exp_stride();
    exp_row_col();
    exp_reuse();

    printf("=== done (sink=%lld) ===\n", g_sink);
    return 0;
}
