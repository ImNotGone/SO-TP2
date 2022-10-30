#include <libs/queueADT.h>

typedef struct node {
    elementType value;
    struct node * next;
} tNode;


struct queueCDT {
    tNode * first;
    tNode * last;
    tNode *current;
};

queueADT newQueue(void){
    queueADT q = calloc(1,sizeof(struct queueCDT));
    q->first = NULL;
    q->last = NULL;
    return q;
}

void toBegin(queueADT q){
    q->current=q->first;
}

int hasNext(queueADT q){
    return q->current!=NULL;
}

// elementType next(queueADT q){
//     if (!hasNext(q)){
//         exit(1);

//     }
//     elementType aux=q->current->value;
//     q->current=q->current->next;
//     return aux;
// }

// Agrega un elemento al final de la cola
void queue(queueADT q, elementType n){
    tNode * aux =malloc(sizeof(tNode));
    aux->value=n;
    aux->next=NULL;
    if (q->first==NULL){
        q->first=aux;
        q->last=aux;
        return;
    }
    q->last->next=aux;
    q->last=aux;

}

elementType * dequeue(queueADT q, elementType * out){
    if (q->first==NULL)
        return NULL;
    *out=q->first->value;
    tNode *aux=q->first;
    q->first=q->first->next;
    free(aux);
    return out;
}

void freeQueueRec(tNode * first){
    if (first==NULL)
        return;
    freeQueueRec(first->next);
    free(first);
}

void freeQueue(queueADT q){
    freeQueueRec(q->first);
    free(q);
}

int isEmpty(queueADT q){
    return q->first==NULL;
}


