#include "Node.h"

void destroy_node(void* node){
    Node *aux = (Node*)node;
    free(aux->key);
    free(aux->value);
    free(aux);
}


Node* create_node_from_K(char* key, int lenkey){ /// para el get y del
    unsigned long slug = hash(key,lenkey);
    return create_node(key,NULL,lenkey,0,slug%TABLESIZE, slug, false );
}

Node* create_node_from_KV(char* key, int lenkey, char *value, int lenvalue){ /// para el put
    unsigned long slug = hash(key,lenkey);
    return create_node(key,value,lenkey,lenvalue,slug%TABLESIZE, slug, printable(value,lenvalue) );
}

Node *create_node(char *key, char* value, int lenkey, int lenvalue, int hash,unsigned long slug,bool printable){
    Node *node = malloc( sizeof(Node) );
    node->key = key;
    node->value = value;
    node->lenkey = lenkey;
    node->lenvalue = lenvalue;
    node-> hash = hash;
    node-> slug = slug;
    node -> printable = printable;
    return node;
}

bool equal_keys(void *na, void *nb){
    Node *a = (Node*)na;
    Node *b = (Node*)nb;
    bool samelen = (a->lenkey == b->lenkey);
    bool sameslug = (a->slug == b->slug);

    if(samelen && sameslug)
        return ( strcmp(a->key, b->key) == 0);

    return false;
}