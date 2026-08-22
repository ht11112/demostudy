/* valid_parentheses.c - LeetCode 20 (local runnable version) */
#include <stdio.h>
#include <stdbool.h>

bool isValid(char *s) {
    char stack[10000];
    int top = -1;
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{') {
            stack[++top] = c;          /* push */
        } else {
            if (top == -1) return false;          /* empty stack, no match */
            char t = stack[top--];                /* pop */
            if ((c == ')' && t != '(') ||
                (c == ']' && t != '[') ||
                (c == '}' && t != '{')) return false;
        }
    }
    return top == -1;                   /* must be empty at the end */
}

int main(void) {
    printf("()[]{}  -> %d\n", isValid("()[]{}"));
    printf("(]      -> %d\n", isValid("(]"));
    printf("(()     -> %d\n", isValid("(()"));
    printf(")       -> %d\n", isValid(")"));
    printf("({[]})  -> %d\n", isValid("({[]})"));
    return 0;
}