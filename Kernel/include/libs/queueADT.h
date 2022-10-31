#ifndef QUEUEADT_H
#define QUEUEADT_H

#include <libs/processManager.h>
#include <libs/memoryManager.h>

typedef struct queueCDT * queueADT;

typedef Pdata elementType;

queueADT newQueue(void);

// Agrega un elemento al final de la cola
void queue(queueADT q, elementType n);

//poppea el primer elemento de la cola
elementType * dequeue(queueADT q, elementType * out);

// Remueve un elemento del principio de la cola y lo deja en out void dequeue(queueADT q, elementType * out);
void freeQueue(queueADT q);

int isEmpty(queueADT q);

void toBegin(queueADT q);

int hasNext(queueADT q);

//elementType next(queueADT q);

#endif //QUEUEADT_H
