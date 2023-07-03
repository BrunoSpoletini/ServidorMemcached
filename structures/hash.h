#ifndef __HASH_H__
#define __HASH_H__

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "../common.h"
#include "dlist.h"
#include "stats.h"
#include "Node.h"

struct Hashtable{
    struct DList *row[TABLESIZE];
    pthread_mutex_t rlock[TABLESIZE];
    struct DList *LRU;
    pthread_mutex_t locklru;
    struct Stats *stats;
};

typedef char *(*FuncionObtencion) (void *dato);

// Crea una tabla hash vacia de tamaño TABLESIZE
Hashtable *create_table();

// Intenta alocar memoria con malloc. En caso de no poder, llama a evict hasta poder.
void *tryalloc(Hashtable *ht, unsigned bytes);

// Devuelve si se pudo eliminar algun nodo segun la politica de desalojo.
bool evict(Hashtable *ht);

// Recibe una string, su tamaño y devuelve un número entre 0 y TABLESIZE
unsigned long long int hash_string(char *value, int size);

// Inserta el nodo en la tabla hash
int _PUT(Hashtable *ht, Node *node);

// Busca un nodo que coincida en clave con el nodo dado, y devuelve su valor.
int _GET(Hashtable *ht, Node *node, char** retval, int *size, bool *printable);

// Busca un nodo en la taabla que coincida en clave con el nodo dado, y lo elimina.
int _DEL(Hashtable *ht,Node *node);

#endif                          /* __HASH_H__ */
