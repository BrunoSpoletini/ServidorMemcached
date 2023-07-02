#include "stats.h"

/*
static inline void inclLock(int *p) {
    asm("lock; incl %0" : "+m"(*p) : : "memory");
}


static inline void declLock(int *p) {
    asm("lock; decl %0" : "+m"(*p) : : "memory");
}
*/

Stats *create_stats(Hashtable *ht){
    Stats *newstats = tryalloc(ht, sizeof(Stats) );
    if(newstats == NULL)
        return NULL;
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

int snapshot_stats(Hashtable *ht, Stats* snapshot){
    Stats *s = ht->stats;
    pthread_mutex_lock(&s->lock);
    snapshot->dels = s->gets;
    snapshot->puts = s->gets;
    snapshot->gets = s->gets;
    snapshot->keys = s->gets;
    pthread_mutex_unlock(&s->lock);
    return OK;
}

void destroy_stats(Stats* s){
    free(s);
}
