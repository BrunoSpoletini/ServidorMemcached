#ifndef __HASH_H__
#define __HASH_H__

#include "dlist.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "dlist.h"
#include "stats.h"
#include "Node.h"

#define TABLESIZE  50000


typedef struct {
    DList *row[TABLESIZE];
    pthread_mutex_t rlock[TABLESIZE];
    DList *LRU;
    pthread_mutex_t locklru;
    Stats *stats;
}Hashtable;



typedef char *(*FuncionObtencion) (void *dato);

// Recibe una string y devuelve un número entre 0 y TABLESIZE
int hash_string(char *value);

// Crea una tabla hash vacia de tamaño TABLESIZE
void inicializar_tabla(Hashtable *ht);



#endif                          /* __HASH_H__ */
