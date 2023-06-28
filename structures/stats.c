#include "stats.h"

/*
static inline void inclLock(int *p) {
    asm("lock; incl %0" : "+m"(*p) : : "memory");
}


static inline void declLock(int *p) {
    asm("lock; decl %0" : "+m"(*p) : : "memory");
}
*/

Stats *create_stats(){
    Stats *newstats = malloc(sizeof(Stats)); /// esto se crea en el mismo momento que la tabla, asi que usamos malloc, y no tryalloc.
    newstats->puts = 0;
    newstats->dels = 0;
    newstats->gets = 0;
    newstats->keys = 0;

    return newstats;
}


void add_put(Stats *s){
    pthread_mutex_lock(&s->lock);
    s->puts++;
    pthread_mutex_unlock(&s->lock);
}

void add_del(Stats *s){
    pthread_mutex_lock(&s->lock);
    s->dels++;
    pthread_mutex_unlock(&s->lock);
}

void add_get(Stats *s){
    pthread_mutex_lock(&s->lock);
    s->gets++;
    pthread_mutex_unlock(&s->lock);
}

void add_key(Stats *s){
    pthread_mutex_lock(&s->lock);
    s->keys++;
    pthread_mutex_unlock(&s->lock);
}

void del_key(Stats *s){
    pthread_mutex_lock(&s->lock);
    s->keys--;
    pthread_mutex_unlock(&s->lock);
}
