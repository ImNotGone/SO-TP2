#ifndef LISTADT_H
#define LISTADT_H

#include <libs/memoryManager.h>
#include <libs/processManager.h>

typedef struct listCDT * listADT;

typedef PCBType * elemType;  	// Tipo de elemento a insertar, por defecto int
/*
** Retorna 0 si los elementos son iguales, negativo si e1 es "menor" que e2 y
** positivo si e1 es "mayor" que e2
*/
static int compare (elemType e1, elemType e2) {
    return e1->pid - e2->pid ;
}

/* Retorna una lista vacía.
*/
listADT listnewList();

/* Agrega un elemento. Si ya estaba no lo agrega */
void listadd(listADT list, elemType elem);

/* Elimina un elemento. */
// void remove(listADT list, elemType elem);

/* Resetea el iterador que recorre la lista en el orden de inserción */
void listtoBegin(listADT list);

/* Retorna 1 si hay un elemento siguiente en el iterador que
** recorre la lista en el orden de inserción. Sino retorna 0 */
int listhasNext(listADT list);

/* Retorna el elemento siguiente del iterador que recorre la lista en el orden de inserción.
** Si no hay un elemento siguiente o no se invocó a toBegin aborta la ejecución.
*/
elemType listnext(listADT list);

/* Resetea el iterador que recorre la lista en forma ascendente */
void listtoBeginAsc(listADT list);

/* Retorna 1 si hay un elemento siguiente en el iterador que
** recorre la lista en forma ascendente. Sino retorna 0 */
int listhasNextAsc(listADT list);

/* Retorna el elemento siguiente del iterador que recorre la lista en forma ascendente.
** Si no hay un elemento siguiente o no se invocó a toBeginAsc aborta la ejecución.
*/
elemType listnextAsc(listADT list);

/* Libera la memoria reservada por la lista */
void listfreeList(listADT list);

#endif //LISTADT_H
