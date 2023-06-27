#include "Node.h"

void destroy_node(Node *node){
    free(node->key);
    free(node->value);
    free(node);
}

Node *create_node(char *key, char* value, int lenkey, int lenvalue, int hash,  bool printable){
    Node *node = malloc( sizeof(Node) );
    node->key = key;
    node->value = value;
    node->lenkey = lenkey;
    node->lenvalue = lenvalue;
    node-> hash = hash;
    node -> printable = printable;
    return node;
}

bool equal_keys(Node *a, Node *b){
    bool samelen = a->lenkey == b->lenkey;
    bool sameslug = a->slug == b->slug;

    if(samelen && sameslug)
        return (strcmp(a->key, b->key) == 0);

    return false;
}