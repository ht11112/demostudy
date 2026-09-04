#include <stdio.h>
#include <stdlib.h>
struct Node { int val; struct Node *left, *right; };
struct Node *new_node(int v) {
    struct Node *n = malloc(sizeof(struct Node));
    n->val = v; n->left = NULL; n->right = NULL;
    return n;
}
int isMirror(struct Node *a, struct Node *b) {
    if (a == NULL && b == NULL) return 1;        // both empty -> ok
    if (a == NULL || b == NULL) return 0;        // only one empty -> not mirror
    return (a->val == b->val)
        && isMirror(a->left, b->right)           // outside vs outside
        && isMirror(a->right, b->left);          // inside vs inside
}
int isSymmetric(struct Node *root) {
    if (root == NULL) return 1;
    return isMirror(root->left, root->right);
}

int main(){
    /* test 1: symmetric tree -> expect 1 */
    struct Node *r = new_node(1);
    r->left = new_node(2);  r->right = new_node(2);
    r->left->left = new_node(3);  r->left->right = new_node(4);
    r->right->left = new_node(4);  r->right->right = new_node(3);
    printf("symmetric tree: %d (expect 1)\n", isSymmetric(r));

    /* test 2: asymmetric tree -> expect 0 */
    struct Node *a = new_node(1);
    a->left = new_node(2);  a->right = new_node(2);
    a->left->right = new_node(3);  a->right->right = new_node(3);
    printf("asymmetric tree: %d (expect 0)\n", isSymmetric(a));

    struct Node *b = NULL;
    printf("empty tree: %d (expect 1)\n", isSymmetric(b));
    return 0;
}