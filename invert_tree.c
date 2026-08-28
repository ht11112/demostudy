#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>


struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct TreeNode* create_node(int val)
{
    struct TreeNode *n = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    n->val = val;
    n->left = NULL;
    n->right = NULL;
    return n;
}

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

void preorder(struct TreeNode *root)
{
    if (root==NULL) return;
    printf("%d ", root->val);
    preorder(root->left);
    preorder(root->right);
}

int main(){
    struct TreeNode *root = create_node(1);
    root->left  = create_node(2);
    root->right = create_node(3);
    root->left->left = create_node(4);   // 树：1(2,3)  2(4)

    printf("Original tree (preorder): ");
    preorder(root);
    printf("\n");

    root = invert_tree(root);

    printf("Inverted tree (preorder): ");
    preorder(root);
    printf("\n");

    return 0;
}