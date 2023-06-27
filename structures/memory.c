#include "memory.h"

void tryalloc(unsigned bytes, Hashtable *ht){

    int space = malloc(bytes);
    if(space == NULL){
         return evict(ht,bytes);
    }

    return space;
}
