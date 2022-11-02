#include <lib.h>
#include <ADTS/queueADT.h>

typedef struct node {
    void * elem;
    struct node * tail;
} tNode;

typedef tNode * tList;

typedef struct queueCDT {
    size_t elementSize;
    tList first;
    tList last;
    tList iter;
} queueCDT;

queueADT newQueue(size_t elementSize) {
    queueADT q = calloc(1, sizeof(queueCDT));
    q->elementSize = elementSize;
    return q;
}

bool queue(queueADT q, void * elem) {
    if(q == NULL || elem == NULL) {
        return false;
    }
    tList node = malloc(sizeof(tNode));
    if(node == NULL || (node->elem = malloc(q->elementSize)) == NULL) {
        free(node);
        return false;
    }
    memcpy(node->elem, elem, q->elementSize);
    node->tail = NULL;
    if(isEmpty(q)) {
        q->first = node;
        q->last = q->first;
        return true;
    }
    q->last->tail = node;
    q->last = q->last->tail;
    return true;
}

bool dequeue(queueADT q, void * out) {
    if(q == NULL || isEmpty(q) || out == NULL) {
        return false;
    }
    memcpy(out, q->first->elem, q->elementSize);
    tList aux = q->first;
    q->first = aux->tail;
    free(aux->elem);
    free(aux);
    return true;
}

bool isEmpty(queueADT q) {
    if(q == NULL) return true;
    return q->first == NULL;
}

static void freeList(tList first) {
    if(first == NULL) return;
    freeList(first->tail);
    free(first->elem);
    free(first);
}

void freeQueue(queueADT q) {
    if(q == NULL) return;
    freeList(q->first);
    free(q);
}

void toBegin(queueADT q) {
    if(q == NULL) return;
    q->iter = q->first;
}

bool hasNext(queueADT q) {
    if(q == NULL) return false;
    return q->iter != NULL;
}

bool next(queueADT q, void * out) {
    if(q == NULL || !hasNext(q) || out == NULL) {
        return false;
    }
    memcpy(out, q->iter->elem, q->elementSize);
    q->iter = q->iter->tail;
    return true;
}
