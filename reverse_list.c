#include<stdio.h>
#include<stdlib.h>
//反转链表
//1.定义链表节点结构体
struct Node {
    int data;
    struct Node* next;
};
//2.反转链表函数
struct Node* reverseList(struct Node* head) {
    struct Node* prev = NULL;
    struct Node* current = head;
    struct Node* temp = NULL;

    while (current != NULL) {
        temp = current->next;
        current->next = prev;
        prev = current;
        current = temp;
    }
    return prev;
}

//3.创建链表函数
struct Node* createList(int v) {
    struct Node* head = (struct Node*)malloc(sizeof(struct Node));
    head->data = v;
    head->next = NULL;
    return head;
}

//4.打印链表函数
void printList(struct Node* head) {
    struct Node* current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

int main(){
 struct Node* head = createList(1);
    head->next = createList(2);
    head->next->next = createList(3);
    head->next->next->next = createList(4);
    head->next->next->next->next = createList(5);

    printf("Original List: ");
    printList(head);

    head = reverseList(head);

    printf("Reversed List: ");
    printList(head);

    return 0;
}