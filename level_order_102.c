/* level_order_102.c - LeetCode 102 submission version + local test
 * Submit ONLY the levelOrder() function (LeetCode already defines TreeNode).
 */

#include <stdio.h>
#include <stdlib.h>

/* ---- LeetCode environment already has this struct; needed only for local test ---- */
struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

/* ==================== LeetCode 102 submission version ==================== */
#define MAXN 10000

int** levelOrder(struct TreeNode* root, int* returnSize, int** returnColumnSizes) {
    *returnSize = 0;
    if (root == NULL) {
        *returnColumnSizes = NULL;
        return NULL;
    }

    struct TreeNode *q[MAXN];        /* array used as a queue (indices only increase) */
    int front = 0, rear = 0;
    int **ans = malloc(sizeof(int*) * MAXN);        /* rows (one per level) */
    *returnColumnSizes = malloc(sizeof(int) * MAXN); /* width of each row */

    q[rear++] = root;
    while (front < rear) {
        int levelSize = rear - front;              /* nodes in current level */
        int *row = malloc(sizeof(int) * levelSize);
        for (int i = 0; i < levelSize; i++) {
            struct TreeNode *cur = q[front++];     /* dequeue */
            row[i] = cur->val;
            if (cur->left)  q[rear++] = cur->left; /* enqueue children */
            if (cur->right) q[rear++] = cur->right;
        }
        (*returnColumnSizes)[*returnSize] = levelSize;
        ans[(*returnSize)++] = row;
    }
    return ans;
}

/* ==================== local test only (NOT for submission) ==================== */
struct TreeNode *tn(int v) {
    struct TreeNode *n = malloc(sizeof(struct TreeNode));
    n->val = v;
    n->left = NULL;
    n->right = NULL;
    return n;
}

int main(void) {
    struct TreeNode *r = tn(1);
    r->left = tn(2);  r->right = tn(3);
    r->left->left = tn(4);  r->left->right = tn(5);

    int returnSize = 0;
    int *returnColumnSizes = NULL;
    int **ans = levelOrder(r, &returnSize, &returnColumnSizes);

    for (int i = 0; i < returnSize; i++) {
        for (int j = 0; j < returnColumnSizes[i]; j++)
            printf("%d ", ans[i][j]);
        printf("\n");
    }
    return 0;
}
