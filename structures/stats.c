#include "stats.h"


static inline void inclLock(int *p) {
    asm("lock; incl %0" : "+m"(*p) : : "memory");
}


static inline void declLock(int *p) {
    asm("lock; decl %0" : "+m"(*p) : : "memory");
}


void add_put(Stats *s){
    inclLock(&s->puts);
}

void add_del(Stats *s){
    inclLock(&s->dels);
}

void add_get(Stats *s){
    inclLock(&s->gets);
}

void add_key(Stats *s){
    inclLock(&s->keys);
}

void del_key(Stats *s){
    declLock(&s->keys);
}
