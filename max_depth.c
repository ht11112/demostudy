#include <stdio.h>
#include <stdlib.h>

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct TreeNode *new_node(int val)
{
    struct TreeNode *p = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    p->val = val;
    p->left = NULL;
    p->right = NULL;
    return p;
}

/* max_depth: 1 + max(left, right), base case NULL = 0 */
int max_depth(struct TreeNode *root)
{
    if (root == NULL) {          /* stop condition: 空树高度 0 */
        return 0;
    }
    int left = max_depth(root->left);    /* 先算左 */
    int right = max_depth(root->right);  /* 再算右 */
    return (left > right ? left : right) + 1;
}

int main(void)
{
    /* build:     1
                 / \
                2   3
               / \
              4   5     -> height = 3 */
    struct TreeNode *root = new_node(1);
    root->left = new_node(2);
    root->right = new_node(3);
    root->left->left = new_node(4);
    root->left->right = new_node(5);

    printf("max depth = %d\n", max_depth(root));   /* expect 3 */
    return 0;
}
