#ifndef __STATS_H__
#define __STATS_H__
#include <pthread.h>
#include <stdlib.h>
#include "../common.h"
#include "hash.h"

struct Stats{

    unsigned long long puts, dels, gets,keys;

    pthread_mutex_t lock; /// no solo para sumar, sino que al momento de imprimir, tambien hay que usarlo.
};


Stats *create_stats();
int snapshot_stats(Hashtable *ht, Stats* snapshot);
void destroy_stats();
void add_put(Stats *s);
void add_del(Stats *s);
void add_get(Stats *s);
void add_key(Stats *s);
void del_key(Stats *s);



#endif