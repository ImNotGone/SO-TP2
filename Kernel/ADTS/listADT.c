#include <ADTS/listADT.h>
#include <stdbool.h>

typedef struct node {
    elemType elem;
    struct node * tail;
} tNode;

typedef tNode * tList;

typedef struct listCDT {
    size_t size;
    // listas
    tList list_asc;
    tList list_ord;
    // auxiliares
    tList last_ord;
    tList curr_asc;
    tList curr_ord;
} listCDT;

listADT listnewList() {
    return calloc(1, sizeof(listCDT));
}

static tList listinsertNode(elemType elem, tList tail) {
    tList aux = malloc(sizeof(tNode));
    aux->elem = elem;
    aux->tail = tail;
    return aux;
}

static tList listaddRec(tList list, elemType elem, bool * flag) {
    int c;
    if(list == NULL || (c = compare(list->elem, elem)) > 0) {
        *flag = true;
        return listinsertNode(elem, list);
    }
    if(c == 0)
        return list;
    list->tail = listaddRec(list->tail, elem, flag);
    return list;
}

void listadd(listADT list, elemType elem) {
    bool flag = false;
    list->list_asc = listaddRec(list->list_asc, elem, &flag);
    if(flag) {
        list->size++;
        if(list->list_ord == NULL) {
            list->list_ord = listinsertNode(elem, NULL);
            list->last_ord = list->list_ord;
            return;
        }
        list->last_ord->tail = listinsertNode(elem, NULL);
        list->last_ord = list->last_ord->tail;
    }
}
void listtoBegin(listADT list) {
    list->curr_ord = list->list_ord;
}

int listhasNext(listADT list) {
    if(list->curr_ord != NULL)
        return true;
    return false;
}

elemType listnext(listADT list) {
    if(!listhasNext(list)) {
        //fprintf(stderr, "No habia mas elementos en la lista, utilice hasNext()");
        //exit(1);
    }
    elemType aux = list->curr_ord->elem;
    list->curr_ord = list->curr_ord->tail;
    return aux;
}

void listtoBeginAsc(listADT list) {
    list->curr_asc = list->list_asc;
}

int listhasNextAsc(listADT list) {
    if(list->curr_asc != NULL)
        return true;
    return false;
}

elemType listnextAsc(listADT list) {
    if(!listhasNextAsc(list)) {
        //fprintf(stderr, "No habia mas elementos en la lista, utilice hasNext()");
        //exit(1);
    }
    elemType aux = list->curr_asc->elem;
    list->curr_asc = list->curr_asc->tail;
    return aux;
}
static void listfreeRec(tList list) {
    if(list == NULL)
        return;
    listfreeRec(list->tail);
    free(list);
}

void listfreeList(listADT list) {
    listfreeRec(list->list_ord);
    listfreeRec(list->list_asc);
    free(list);
}
