#include <stdio.h>
#include <stdlib.h>   /* abs() 和 malloc() 都在这里 */

int* sortedSquares(int* nums, int numsSize, int* returnSize) {
    int* ans = (int*)malloc(numsSize * sizeof(int));   /* 结果数组 */
    if (ans == NULL) { printf("malloc failed\n"); return NULL; }
    int l = 0, r = numsSize - 1, k = numsSize - 1;     /* l/r 两端，k 从最后往前 */
    while (l <= r) {                                    /* 注意是 <=，最后一个也要填 */
        if (abs(nums[l]) > abs(nums[r])) {              /* 比绝对值，不是比原值！ */
            ans[k--] = nums[l] * nums[l];
            l++;                                        /* 左边被取走，l 往中间挪 */
        } else {
            ans[k--] = nums[r] * nums[r];
            r--;                                        /* 右边被取走，r 往中间挪 */
        }
    }
    *returnSize = numsSize;
    return ans;
}

int main(void) {
    int a[] = {-4, -1, 0, 3, 10};
    int ret = 0;
    int* res = sortedSquares(a, 5, &ret);
    for (int i = 0; i < ret; i++)
        printf("res[%d]=%d\n", i, res[i]);
    free(res);      /* 用完了要 free（你自己的内存自己管） */
    return 0;
}