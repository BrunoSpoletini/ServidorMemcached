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
#include "../common.h"



typedef struct {
    struct DList *row[TABLESIZE];
    pthread_mutex_t rlock[TABLESIZE];
    struct DList *LRU;
    pthread_mutex_t locklru;
    struct Stats *stats;
}Hashtable;



typedef char *(*FuncionObtencion) (void *dato);

// Crea una tabla hash vacia de tamaño TABLESIZE
void inicializar_tabla(Hashtable *ht);

void *tryalloc(Hashtable *ht, unsigned bytes);

bool evict(Hashtable *ht);

// Recibe una string y devuelve un número entre 0 y TABLESIZE
int hash_string(char *value);

void *tryalloc(Hashtable *ht, unsigned bytes);
char *copycat(Hashtable *ht,char *s, int len);
int _PUT(Hashtable *ht, Node *node);
int _GET(Hashtable *ht, Node *node, char* retval);
int _DEL(Hashtable *ht,Node *node);

#endif                          /* __HASH_H__ */
