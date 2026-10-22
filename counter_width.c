#include <stdio.h>

/*
 * Lesson: how many bits do we need for a count?
 *   16-bit counter  -> 65536 clocks before wrap
 *   32-bit counter  -> 4294967296 clocks
 * Rule: timer_period = clock_hz * seconds, and that value must fit the counter width.
 */

#define COUNTER_BITS_16 16
#define COUNTER_BITS_32 32

/* simulate a counter of given width, return clocks until wrap (cap at limit) */
static unsigned long long clocks_until_wrap(int bits, unsigned long long limit)
{
    unsigned long long max_value = 0ULL;
    unsigned long long count = 0ULL;

    /* 2^bits - 1 */
    max_value = (1ULL << bits) - 1ULL;
    while (count < limit) {
        count = (count + 1ULL) & max_value;   /* mask keeps it inside the width */
        if (count == 0ULL) {
            return count;                     /* wrapped, value rolled to 0 */
        }
    }
    return count;
}

static void show_requirement(const char *name, unsigned long long clk_hz, unsigned long long seconds)
{
    unsigned long long need = 0ULL;
    unsigned long long max16 = 0ULL;
    unsigned long long max32 = 0ULL;

    need = clk_hz * seconds;
    max16 = (1ULL << COUNTER_BITS_16) - 1ULL;
    max32 = (1ULL << COUNTER_BITS_32) - 1ULL;

    printf("%-10s clk=%llu Hz, need %llu ticks -> ", name, clk_hz, need);
    if (need <= max16) {
        printf("fits in 16-bit (max %llu)\n", max16);
    } else if (need <= max32) {
        printf("NEEDS 32-bit (16-bit max %llu)\n", max16);
    } else {
        printf("NEEDS prescaler even for 32-bit\n");
    }
}

int main(void)
{
    unsigned long long w16;
    unsigned long long w32;

    printf("-- actual wrap test (brute force) --\n");
    w16 = clocks_until_wrap(COUNTER_BITS_16, 100000ULL);
    printf("16-bit counter wraps after 65536 clocks: got %llu %s\n",
           w16, w16 == 0ULL ? "PASS" : "CHECK");

    w32 = clocks_until_wrap(COUNTER_BITS_32, 70000ULL);
    printf("32-bit counter at 70000 clocks (no wrap, expect 70000): got %llu %s\n",
           w32, w32 == 70000ULL ? "PASS" : "FAIL");

    printf("\n-- 16 bit vs 32 bit requirement table --\n");
    show_requirement("1kHz/1s", 1000ULL, 1ULL);
    show_requirement("72MHz/1s", 72000000ULL, 1ULL);
    show_requirement("72MHz/60s", 72000000ULL, 60ULL);

    printf("\n-- why prescaler: 72MHz/1s = 72000000 ticks > 65535 --\n");
    printf("prescaler 1024 -> 72000000/1024 = %llu ticks (fits 16-bit)\n",
           72000000ULL / 1024ULL);

    return 0;
}
