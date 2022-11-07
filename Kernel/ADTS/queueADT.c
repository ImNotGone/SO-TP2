// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <ADTS/queueADT.h>

typedef struct node {
    void * elem;
    struct node * tail;
} tNode;

typedef tNode * tList;

typedef struct queueCDT {
    uint64_t elementSize;
    compareFp compareFun;
    uint64_t size;
    tList first;
    tList last;
    tList iter;
} queueCDT;

queueADT newQueue(uint64_t elementSize, compareFp compareFun) {
    queueADT q = calloc(1, sizeof(queueCDT));

    if (q == NULL) {
        return NULL;
    }

    q->elementSize = elementSize;
    q->compareFun = compareFun;
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
    q->size++;
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
    q->size--;
    return true;
}

bool remove(queueADT q, void *elem) {
    if(q == NULL || elem == NULL) {
        return false;
    }
    tList prev = NULL;
    tList aux = q->first;
    while(aux != NULL) {

        if(q->compareFun(aux->elem, elem) == 0) {
            memcpy(elem, aux->elem, q->elementSize);
            if(prev == NULL) {
                q->first = aux->tail;
            } else {
                prev->tail = aux->tail;
            }
            free(aux->elem);
            free(aux);
            q->size--;
            return true;
        }

        prev = aux;
        aux = aux->tail;
    }
    return false;
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

uint64_t size(queueADT q) {
    if(q == NULL) return 0;
    return q->size;
}
