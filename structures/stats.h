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

// Crea una variable Stats, y pide memoria de ser necesario a una tabla hash.
Stats *create_stats(Hashtable *ht);

// Toma una instantanea de las Stats de una tabla hash.
int snapshot_stats(Hashtable *ht, Stats* snapshot);

// Elimina la memoria asociada a una variable Stats.
void destroy_stats();

// Agrega un comando put a las estadisticas.
void add_put(Stats *s);

// Agrega un comando del a las estadisticas.
void add_del(Stats *s);

// Agrega un comando get a las estadisticas.
void add_get(Stats *s);

// Agrega una key a las estadisticas.
void add_key(Stats *s);

// Elimina una key de las estadisticas.
void del_key(Stats *s);

#endif