#include <stdio.h>
#include <stdlib.h>

struct Node {
    int val;
    struct Node *left;
    struct Node *right;
};

struct Node *new_node(int v) {
    struct Node *n = malloc(sizeof(struct Node));
    n->val = v;
    n->left = NULL;
    n->right = NULL;
    return n;
}
int minDepth(struct Node* root) {
    if (root == NULL) return 0;                                    // empty -> 0
    if (root->left == NULL) return minDepth(root->right) + 1;      // left empty -> go right
    if (root->right == NULL) return minDepth(root->left) + 1;      // right empty -> go left
    int l = minDepth(root->left);
    int r = minDepth(root->right);
    return (l < r ? l : r) + 1;                                    // both sides -> smaller+1
}


int main(void) {
    /* test 1: full tree -> expect 2 */
    struct Node *r = new_node(1);
    r->left = new_node(2);  r->right = new_node(3);
    r->left->left = new_node(4);  r->left->right = new_node(5);
    printf("full tree: %d (expect 2)\n", minDepth(r));

    /* test 2: chain tree, each node only right child -> expect 3 */
    struct Node *c = new_node(1);
    c->right = new_node(2);
    c->right->right = new_node(3);
    printf("chain tree: %d (expect 3)\n", minDepth(c));

    /* test 3: single node -> expect 1 */
    struct Node *s = new_node(7);
    printf("single node: %d (expect 1)\n", minDepth(s));

    return 0;
}



