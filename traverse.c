#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node *left;
    struct Node *right;
};

struct Node* create_node(int val)
{
    struct Node *n = (struct Node*)malloc(sizeof(struct Node));
    n->data = val;
    n->left = NULL;
    n->right = NULL;      /* 铁律：指针成员必须初始化成 NULL */
    return n;
}

/* 前序 PRE：根 -> 左 -> 右 */
void preorder(struct Node *root)
{
    if (root == NULL) return;          /* 统一骨架：先判空 */
    printf("%d ", root->data);         /* ① 先打印自己 */
    preorder(root->left);              /* ② 再走左边 */
    preorder(root->right);             /* ③ 最后走右边 */
}

/* 中序 IN：左 -> 根 -> 右 */
void inorder(struct Node *root)
{
    if (root == NULL) return;
    inorder(root->left);               /* ① 先走左边 */
    printf("%d ", root->data);         /* ② 回来打印自己 */
    inorder(root->right);              /* ③ 再走右边 */
}

/* 后序 POST：左 -> 右 -> 根 */
void postorder(struct Node *root)
{
    if (root == NULL) return;
    postorder(root->left);             /* ① 先走左边 */
    postorder(root->right);            /* ② 再走右边 */
    printf("%d ", root->data);         /* ③ 最后打印自己 */
}

void free_tree(struct Node *root)
{
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main()
{
    /*        1
           /     \
          2       3
         / \     / \
        4   5   6   7   */
    struct Node *root = create_node(1);
    root->left  = create_node(2);
    root->right = create_node(3);
    root->left->left   = create_node(4);
    root->left->right  = create_node(5);
    root->right->left  = create_node(6);
    root->right->right = create_node(7);

    printf("preorder : ");
    preorder(root);
    printf("\n");

    printf("inorder  : ");
    inorder(root);
    printf("\n");

    printf("postorder: ");
    postorder(root);
    printf("\n");

    free_tree(root);
    return 0;
}
