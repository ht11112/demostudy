#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct TreeNode *newNode(int v) {
    struct TreeNode *n = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    if (n == NULL) return NULL;
    n->val = v;
    n->left = NULL;
    n->right = NULL;
    return n;
}

/* inorder walk with prev pointer: BST <-> strict increasing sequence */
static bool inorder(struct TreeNode *root, struct TreeNode **prev) {
    if (root == NULL) return true;
    if (!inorder(root->left, prev)) return false;
    if (*prev != NULL && root->val <= (*prev)->val) return false; /* not strict */
    *prev = root;
    return inorder(root->right, prev);
}

bool isValidBST(struct TreeNode *root) {
    struct TreeNode *prev = NULL;   /* no INT_MIN trap: first node always ok */
    return inorder(root, &prev);
}

void freeTree(struct TreeNode *root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main(void) {
    /* t1: [2,1,3] valid -> expect 1 */
    struct TreeNode *t1 = newNode(2);
    t1->left = newNode(1);
    t1->right = newNode(3);
    printf("t1 = %d (expect 1)\n", isValidBST(t1));
    freeTree(t1);

    /* t2: [5,1,4,NULL,NULL,3,6] invalid (lc sample) -> expect 0 */
    struct TreeNode *t2 = newNode(5);
    t2->left = newNode(1);
    t2->right = newNode(4);
    t2->right->left = newNode(3);
    t2->right->right = newNode(6);
    printf("t2 = %d (expect 0)\n", isValidBST(t2));
    freeTree(t2);

    /* t3: [5,4,6,NULL,NULL,3,7] trap -> expect 0
       3 < 6 locally ok, but 3 sits in right subtree of 5, must be > 5 */
    struct TreeNode *t3 = newNode(5);
    t3->left = newNode(4);
    t3->right = newNode(6);
    t3->right->left = newNode(3);
    t3->right->right = newNode(7);
    printf("t3 = %d (expect 0)\n", isValidBST(t3));
    freeTree(t3);

    /* t4: single node [INT_MIN] valid -> expect 1 (proves no INT_MIN trap) */
    struct TreeNode *t4 = newNode(-2147483647 - 1);
    printf("t4 = %d (expect 1)\n", isValidBST(t4));
    freeTree(t4);

    return 0;
}
