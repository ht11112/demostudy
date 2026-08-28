#include <stdio.h>
#include <stdlib.h>

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

/* 建节点统一入口：val 赋值，left/right 清零（铁律） */
struct TreeNode* create_node(int val)
{
    struct TreeNode *n = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    n->val = val;
    n->left = NULL;
    n->right = NULL;
    return n;
}

/* 你的反转函数，原封不动，是对的 */
struct TreeNode* invert_tree(struct TreeNode* root)
{
    if (root == NULL) {
        return NULL;
    }
    struct TreeNode *tmp = root->left;
    root->left = invert_tree(root->right);
    root->right = invert_tree(tmp);
    return root;
}

/* 前序遍历，用来验证 */
void preorder(struct TreeNode *root)
{
    if (root == NULL) return;
    printf("%d ", root->val);
    preorder(root->left);
    preorder(root->right);
}

/* 递归释放，呼应"先判空再递归"骨架 */
void free_tree(struct TreeNode *root)
{
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main()
{
    struct TreeNode *root = create_node(1);
    root->left  = create_node(2);
    root->right = create_node(3);
    root->left->left = create_node(4);   // 树：1(2,3)  2(4)

    printf("before: ");
    preorder(root);
    printf("\n");

    root = invert_tree(root);

    printf("after : ");
    preorder(root);
    printf("\n");

    free_tree(root);
    return 0;
}
