#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
struct Node { int val; struct Node *left, *right; };
struct Node *new_node(int v) {
    struct Node *n = malloc(sizeof(struct Node));
    n->val = v; n->left = NULL; n->right = NULL;
    return n;
}
/* iterative insert: walk by value, hang new node at first empty slot */
struct Node *insert(struct Node *root, int v) {
    if (root == NULL) return new_node(v);        // empty tree -> new root
    struct Node *cur = root;
    while (1) {
        if (v < cur->val) {
            if (cur->left == NULL) { cur->left = new_node(v); break; }
            cur = cur->left;                      // go left
        } else {
            if (cur->right == NULL) { cur->right = new_node(v); break; }
            cur = cur->right;                     // go right
        }
    }
    return root;
}
void inorder(struct Node *r) {                    // reuse from last month
    if (!r) return;
    inorder(r->left);
    printf("%d ", r->val);
    inorder(r->right);
}
int main() {
    struct Node *r = NULL;
    int a[] = {5, 3, 7, 2, 4, 6, 8, 1};           // insert 1 at last
    for (int i = 0; i < 8; i++) r = insert(r, a[i]);
    inorder(r);                                   // expect: 1 2 3 4 5 6 7 8
    return 0;
}
