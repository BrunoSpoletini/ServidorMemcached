#ifndef __NODE_H__
#define __NODE_H__

#include "../common.h"
#include "../utils.h"

struct Node{
    char * key, *value;
    int lenkey, lenvalue;
    unsigned long long slug; 
    int hash; /// el hash es el slug % tamaño de la tabla. Nos sirve para evitar comparaciones.
    bool printable;
};

// Libera la memoria asociada a un nodo.
void destroy_node(void* node);

// Crea un nodo a partil de su clave
Node* create_node_from_K(Hashtable *ht,char* key, int lenkey);

// Crea un nodo a paratir de su clave y su valor.
Node* create_node_from_KV(Hashtable *ht,char* key, int lenkey, char *value, int lenvalue);

// Crea un nodo segun todos sus parametros.
Node *create_node(Hashtable *ht,char *key, char* value, int lenkey, int lenvalue, int hash,unsigned long long slug,bool printable);

// Compara eficientemente si dos nodos tienen la misma clave.
bool equal_keys(void *na, void *nb);

#endif