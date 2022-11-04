// C code to implement Priority Queue
// using Linked List

#include <ADTS/priorityQueueADT.h>

// Node
typedef struct node {
	void * data;

	// Lower values indicate higher priority
	void * priority;

	struct node* next;

} Node;

typedef struct pQueueCDT {
    Node* front;

    compareFp compare;

    uint64_t elementSize;
    uint64_t prioritySize;


} pQueueCDT;

// Function to Create A New priorityQueue
pQueueADT newPQueue(compareFp compare, uint64_t elementSize, uint64_t prioritySize) {
    pQueueADT pQueue = calloc(1, sizeof(pQueueCDT));

    pQueue->front = NULL;
    pQueue->compare = compare;
    pQueue->elementSize = elementSize;
    pQueue->prioritySize = prioritySize;

    return pQueue;
}

// Return the value at head
bool peekPq(pQueueADT pQueue, void* element) {
    if (pQueue->front == NULL) {
        return false;
    }

    memcpy(element, pQueue->front->data, pQueue->elementSize);

    return true;
}

// Removes the element with the
// highest priority from the list
bool popPq(pQueueADT pQueue, void* element) {
    Node *ptr;
    if (pQueue->front == NULL) {
        return false;
    }
    else {
        // Assign front node to ptr
        ptr = pQueue->front;

        // Assign second node to front
        pQueue->front = pQueue->front->next;

        if (element != NULL) {
            memcpy(element, ptr->data, pQueue->elementSize);
        }

        // Free memory of previous front node
        free(ptr->data);
        free(ptr->priority);
        free(ptr);
    }
    return true;
}

// Function to push according to priority
bool pushPq(pQueueADT pQueue, void * elem, void * priority) {
    Node *start = (Node *)pQueue->front;

    // Create new Node
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = malloc(pQueue->elementSize);
    temp->priority = malloc(pQueue->prioritySize);
    memcpy(temp->data, elem, pQueue->elementSize);
    memcpy(temp->priority, priority, pQueue->prioritySize);

    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if (pQueue->front == NULL || pQueue->compare(priority, pQueue->front->priority) < 0) {

        // Insert New Node before head
        temp->next = pQueue->front;
        pQueue->front = temp;
    }
    else {

        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
               pQueue->compare(priority, start->next->priority) >= 0) {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
    return true;
}

// Function to check is list is empty
int isEmptyPq(pQueueADT pQueue) {
    return pQueue->front == NULL;
}

// Pops all the elements in the queue that are less than or equal to the given priority
// Returns an array of the elements found
// The size of the array is stored in the size parameter
void * getElementsLessThan(pQueueADT pQueue, void * priority, uint64_t * size) {
    Node * start = pQueue->front;
    uint64_t count = 0;

    if (isEmptyPq(pQueue)) {
        *size = 0;
        return NULL;
    }

    // Count the number of elements that are less than or equal to the given priority
    while (start != NULL && pQueue->compare(priority, start->priority) >= 0) {
        count++;
        start = start->next;
    }

    // Allocate memory for the array
    void * elements = malloc(count * pQueue->elementSize);

    // Reset the start pointer
    start = pQueue->front;

    // Copy the elements into the array
    for (uint64_t i = 0; i < count; i++) {
        memcpy(elements + (i * pQueue->elementSize), start->data, pQueue->elementSize);

        // Remove the element from the queue
        popPq(pQueue, NULL);

        start = start->next;
    }

    *size = count;

    return elements;
}


