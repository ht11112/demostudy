#include<stdio.h>
#include<stdlib.h>

struct Node {
    int data;
    struct Node* next;
};

int main()
 {
    struct Node n1 = {10, NULL}; // Initialize n1 with data=10 and next=NULL
    struct Node n2 = {20, NULL}; // Initialize n2 with data=20
    struct Node n3 = {30, NULL}; // Initialize n3 with data=30
    struct Node n4 = {40, NULL}; // Initialize n4 with data=40
    struct Node n5 = {50, NULL}; // Initialize n5 with data=50
    n1.next = &n2; // Link n1 to n2
    n2.next = &n3; // Link n2 to n3
    n3.next = &n4; // Link n3 to n4
    n4.next = &n5; // Link n4 to n5

    struct Node* p = &n1; // Start from the head of the list
    while (p!=NULL)
    {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("NULL\n");
    return 0;
}