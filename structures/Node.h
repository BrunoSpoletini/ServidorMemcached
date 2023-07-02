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


void destroy_node(void* node);

Node *create_node(Hashtable *ht,char *key, char* value, int lenkey, int lenvalue, int hash,unsigned long long slug,bool printable);

bool equal_keys(void *na, void *nb);

Node* create_node_from_KV(Hashtable *ht,char* key, int lenkey, char *value, int lenvalue);

Node* create_node_from_K(Hashtable *ht,char* key, int lenkey);


#endif