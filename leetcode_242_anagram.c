#include <stdio.h>
#include <string.h>

/* counting array: O(n) time, O(1) space (26 letters) */
static int is_anagram(const char *s, const char *t)
{
    int cnt[26] = { 0 };
    int i;

    if (strlen(s) != strlen(t)) return 0;

    for (i = 0; s[i] != '\0'; i++) {
        cnt[s[i] - 'a']++;
        cnt[t[i] - 'a']--;
    }
    for (i = 0; i < 26; i++)
        if (cnt[i] != 0) return 0;

    return 1;
}

static void run_case(const char *s, const char *t, int expect)
{
    int got = is_anagram(s, t);
    printf("\"%s\" vs \"%s\" -> %d (expect %d) %s\n",
           s, t, got, expect, got == expect ? "PASS" : "FAIL");
}

int main(void)
{
    run_case("anagram", "nagaram", 1);
    run_case("rat", "car", 0);
    run_case("a", "ab", 0);
    run_case("", "", 1);
    run_case("aacc", "ccac", 0);

    return 0;
}
