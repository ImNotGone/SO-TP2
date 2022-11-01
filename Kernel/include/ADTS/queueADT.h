#ifndef QUEUEADT_H
#define QUEUEADT_H

#include <types.h>
#include <libs/memoryManager.h>

typedef struct queueCDT * queueADT;

// Genera una nueva cola
queueADT newQueue(uint64_t elementSize);

// Agrega un elemento al final de la cola
bool queue(queueADT q, void * elem);

// Remueve un elemento del principio de la cola y lo deja en out
bool dequeue(queueADT q, void * out);

// retorna si la queue esta vacia
bool isEmpty(queueADT q);

// libera la estructura
void freeQueue(queueADT q);

/* Funciones para iterar */

void toBegin(queueADT q);

bool hasNext(queueADT q);

bool next(queueADT q, void * out);

#endif //PI_QUEUEADT_H
