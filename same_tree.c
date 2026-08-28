#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

bool isSameTree(struct Node* p, struct Node* q) {
    if (p == NULL && q == NULL) return true;   // 1. 两个都空 → 相同
    if (p == NULL || q == NULL) return false;  // 2. 一个空一个不空 → 不同
    return p->val == q->val                     // 3. 值相同
        && isSameTree(p->left, q->left)         //    且左子树相同
        && isSameTree(p->right, q->right);      //    且右子树相同
}


int main(void) {
    /*     1
          / \
         2   3
        / \
       4   5      */
    Node *p = new_node(1);
    p->left = new_node(2);
    p->right = new_node(3);
    p->left->left = new_node(4);
    p->left->right = new_node(5);

    Node *q = new_node(1);
    q->left = new_node(2);
    q->right = new_node(3);
    q->left->left = new_node(4);
    q->left->right = new_node(5);

    printf("Same tree: %s\n", isSameTree(p, q) ? "1" : "0");
    
   q->left->left->val = 99;   // 把 q 的一处改掉，让两棵树不同
    printf("Different tree: %s\n", isSameTree(p, q) ? "1" : "0");
    return 0;
}