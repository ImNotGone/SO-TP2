#ifndef QUEUEADT_H
#define QUEUEADT_H

#include <types.h>
#include <libs/memoryManager.h>

typedef int64_t (*compareFp)(void * e1, void * e2);
typedef struct queueCDT * queueADT;

// Genera una nueva cola
queueADT newQueue(uint64_t elementSize, compareFp compareFun);

// Agrega un elemento al final de la cola
bool queue(queueADT q, void * elem);

// Remueve un elemento del principio de la cola y lo deja en out
bool dequeue(queueADT q, void * out);

// Busca el elemento, utilizando la compareFun, lo remueve de la cola
// retorna el elemento de la cola en elem y retorna true
// Si no lo encuentra, devuelve false
bool remove(queueADT q, void * elem);

// retorna si la queue esta vacia
bool isEmpty(queueADT q);

// libera la estructura
void freeQueue(queueADT q);

/* Funciones para iterar */

void toBegin(queueADT q);

bool hasNext(queueADT q);

bool next(queueADT q, void * out);

uint64_t size(queueADT q);

#endif //PI_QUEUEADT_H
