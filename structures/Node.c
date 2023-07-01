#include "hash.h"
#include "Node.h"

void destroy_node(void* node){
    Node *aux = (Node*)node;

    if(aux == NULL)
        return;

    if(aux->key != NULL)
        free(aux->key);
    if(aux->value != NULL)
        free(aux->value);
    
    //if(node != NULL)
        //free(node);
}

Node* create_node_from_K(Hashtable *ht,char* key, int lenkey){ /// para el get y del
    unsigned long slug = hash_string(key, lenkey);
    return create_node(ht,key,NULL,lenkey,0,slug%TABLESIZE, slug, false );
}

Node* create_node_from_KV(Hashtable *ht,char* key, int lenkey, char *value, int lenvalue){ /// para el put
    unsigned long slug = hash_string(key, lenkey);
    return create_node(ht,key,value,lenkey,lenvalue,slug%TABLESIZE, slug, printable(value,lenvalue) );
}

Node *create_node(Hashtable *ht,char *key, char* value, int lenkey, int lenvalue, int hash,unsigned long slug,bool printable){
    Node *node = tryalloc( ht , sizeof(Node) );
    if(node == NULL){
        return NULL;
    }
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
        return ( char_arr_cmp(a->key, b->key, a->lenkey) == 0);
    return false;
}