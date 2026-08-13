#include <stdio.h>
#include <stddef.h>

struct A { char c; int i; char d; };   /* char-int-char：中间要填 3 字节 */
struct B { char c; char d; int i; };   /* char-char-int：只填 2 字节 */

int main(void)
{
    printf("sizeof(A)=%zu\n", sizeof(struct A));
    printf("sizeof(B)=%zu\n", sizeof(struct B));
    printf("offsetof(A.c)=%zu\n", offsetof(struct A, c));
    printf("offsetof(A.i)=%zu\n", offsetof(struct A, i));
    printf("offsetof(A.d)=%zu\n", offsetof(struct A, d));
    printf("offsetof(B.c)=%zu\n", offsetof(struct B, c));
    printf("offsetof(B.d)=%zu\n", offsetof(struct B, d));
    printf("offsetof(B.i)=%zu\n", offsetof(struct B, i));
    return 0;
}
