//Created linked list

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct node_var {
    char *x;
    struct node_var *next;
} NodeT;

void insertNode(char *string1);
NodeT *makeNode(char *string1);
void printList();

unsigned int size_list = 0;
NodeT *head = NULL;
NodeT *current = NULL;

NodeT *makeNode(char *string1) {
    
    NodeT *newNode = malloc(sizeof(struct node_var));
    newNode->x = malloc(strlen(string1) + 1);
    strcpy(newNode->x, string1);
    return newNode;
}

void insertNode(char *string1) {

    NodeT *new = makeNode(string1);
    assert(new != NULL);
    size_list++;
    if (head == NULL) {
        head = new;
        new->next = NULL;
    } else {
        NodeT *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
        new->next = NULL;
    }

}

void printList() {

    NodeT *print = head;
    printf("List size: %d\n", size_list);
    while (print->next != NULL) {
        printf("%s->", print->x);
        print = print->next;
    }
    printf("%s\n", print->x);
    
}

void freeList() {
    
    NodeT *free_list = head;
    while (free_list != NULL) {
        NodeT *temp = free_list->next;
        free(free_list->x);
        free(free_list);
        free_list = temp;
    }
}

int main(int argc, char *argv[]) {

    insertNode("hello world");
    insertNode("he1");
    insertNode("practice");
    insertNode("forrest");

    printList();
    freeList();


    return EXIT_SUCCESS;
}









