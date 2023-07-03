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


//crea una variable Stats, y pide memoria de ser necesario a una tabla hash.
Stats *create_stats(Hashtable *ht);

//toma una instantanea de las Stats de una tabla hash.
int snapshot_stats(Hashtable *ht, Stats* snapshot);

//elimina la memoria asociada a una variable Stats.
void destroy_stats();

//agrega un comando put a las estadisticas.
void add_put(Stats *s);

//agrega un comando del a las estadisticas.
void add_del(Stats *s);

//agrega un comando get a las estadisticas.
void add_get(Stats *s);

//agrega una key a las estadisticas.
void add_key(Stats *s);

//elimina una key de las estadisticas.
void del_key(Stats *s);



#endif