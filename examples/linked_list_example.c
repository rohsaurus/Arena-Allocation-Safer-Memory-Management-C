#include <stdio.h>
#include "../arena.h"

typedef struct Node {
    int value;
    struct Node* next;
} Node;

int main() {
    // Initialize the arena with a capacity of 1024 bytes
    Arena* arena = ArenaInit(1024);
    if (!arena) {
        printf("Failed to initialize arena\n");
        return 1;
    }

    // Create a linked list with 5 nodes
    Node* head = (Node*) PushStruct(arena, Node);
    if (!head) {
        printf("Failed to allocate memory for head node\n");
        ArenaFree(arena);
        return 1;
    }
    head->value = 1;
    head->next = NULL;

    Node* current = head;
    for (int i = 2; i <= 5; i++) {
        Node* newNode = (Node*)PushStruct(arena, Node);
        if (!newNode) {
            printf("Failed to allocate memory for node %d\n", i);
            ArenaFree(arena);
            return 1;
        }
        newNode->value = i;
        newNode->next = NULL;
        current->next = newNode;
        current = newNode;
    }

    // Print the linked list
    current = head;
    while (current) {
        printf("Node value: %d\n", current->value);
        current = current->next;
    }

    // Clean up the arena (automatically frees all nodes)
    ArenaFree(arena);

    return 0;
}
