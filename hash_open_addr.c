#include <stdio.h>

#define HASH_SIZE 13   /* prime -> fewer collisions */

typedef struct { int key; int used; } Slot;   /* 0 = empty, 1 = occupied */
static Slot table[HASH_SIZE];

static int hash_index(int key)
{
    return (int)((unsigned)key % HASH_SIZE);   /* cast to unsigned BEFORE modulo */
}

static int ht_insert(int key)
{
    int start = hash_index(key), i;
    for (i = 0; i < HASH_SIZE; i++) {
        int pos = (start + i) % HASH_SIZE;     /* linear probing */
        if (!table[pos].used) {
            table[pos].key = key; table[pos].used = 1;
            printf("insert %4d -> slot %2d (probe %d)\n", key, pos, i);
            return 0;
        }
        if (table[pos].key == key) { printf("insert %4d -> already at slot %2d\n", key, pos); return -1; }
    }
    printf("insert %4d failed: table full\n", key);
    return -1;
}

static int ht_find(int key)
{
    int start = hash_index(key), i;
    for (i = 0; i < HASH_SIZE; i++) {
        int pos = (start + i) % HASH_SIZE;
        if (!table[pos].used) return -1;       /* first empty -> give up */
        if (table[pos].key == key) return pos;
    }
    return -1;
}

static void ht_dump(void)
{
    int i, cnt = 0;
    printf("\n--- table dump ---\n");
    for (i = 0; i < HASH_SIZE; i++)
        if (table[i].used) { printf("slot[%2d] = %4d\n", i, table[i].key); cnt++; }
    printf("load factor = %d/%d = %.2f\n", cnt, HASH_SIZE, (double)cnt / HASH_SIZE);
}

int main(void)
{
    int keys[] = { 15, 28, 5, -5, 41, 2 };
    int n = (int)(sizeof(keys) / sizeof(keys[0])), i;

    printf("HASH_SIZE = %d\n", HASH_SIZE);
    for (i = 0; i < n; i++) ht_insert(keys[i]);
    ht_dump();

    printf("\n--- find ---\n");
    printf("find 28 -> slot %2d\n", ht_find(28));
    printf("find -5 -> slot %2d\n", ht_find(-5));
    printf("find 99 -> slot %2d (not found)\n", ht_find(99));
    printf("\nnaive -5 %% 13 = %d  <-- negative index = out of bounds!\n", -5 % HASH_SIZE);
    return 0;
}
