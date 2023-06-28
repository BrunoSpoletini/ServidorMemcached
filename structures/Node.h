#ifndef __NODE_H__
#define __NODE_H__

#include "../common.h"

typedef struct {
    char * key, *value;
    int lenkey, lenvalue;
    int slug; 
    int hash; /// el hash es el slug % tamaño de la tabla. Nos sirve para evitar comparaciones.
    bool printable;
}Node;


void destroy_node(void* node);
Node *create_node(char *key, char* value, int lenkey, int lenvalue, int hash, bool printable);
bool equal_keys(void *na, void *nb);

#endif