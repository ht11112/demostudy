#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *left;
    struct Node *right;
} Node;

Node *new_node(int v) {
    Node *n = (Node *)malloc(sizeof(Node));
    n->val = v;
    n->left = NULL;
    n->right = NULL;
    return n;
}

void preorder(Node *root) {
    if (root == NULL) return;   // 空树直接返回
    printf("%d ", root->val);
    preorder(root->left);
    preorder(root->right);
}

void inorder(Node *root) {
    if (root == NULL) return;
    inorder(root->left);
    printf("%d ", root->val);
    inorder(root->right);
}

void postorder(Node *root) {
    if (root == NULL) return;
    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->val);
}

int count_nodes(struct Node* root) {
    if (root == NULL) return 0;
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

int count_leaves(struct Node* root) {
    if (root == NULL) return 0;
    if (root->left == NULL && root->right == NULL) return 1;  // 叶子
    return count_leaves(root->left) + count_leaves(root->right);
}

int tree_height(struct Node* root) {
    if (root == NULL) return 0;
    int l = tree_height(root->left);      // 临时变量防重复递归
    int r = tree_height(root->right);
    return (l > r ? l : r) + 1;           // 三目必须加括号！
}

int main(void) {
    /*     1
          / \
         2   3
        / \
       4   5      */
    Node *root = new_node(1);
    root->left = new_node(2);
    root->right = new_node(3);
    root->left->left = new_node(4);
    root->left->right = new_node(5);

    printf("pre:  "); preorder(root); printf("\n");
    printf("in:   "); inorder(root); printf("\n");
    printf("post: "); postorder(root); printf("\n");
    printf("count_nodes = %d\n", count_nodes(root));
    printf("count_leaves = %d\n", count_leaves(root));
    printf("tree_height = %d\n", tree_height(root));
    return 0;
}