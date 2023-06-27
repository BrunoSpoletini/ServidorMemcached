#ifndef __STATS_H__
#define __STATS_H__
#include <pthread.h>

typedef struct {

    unsigned long long puts, dels, gets,keys;

    /// aca se podria agregar un lock, para que al momento de imprimir las stats,
    /// no se permita que otro las edite, y asi evitar tener unas estadisticas "invalidas".

}Stats;


void add_put(Stats *s);
void add_del(Stats *s);
void add_get(Stats *s);
void add_key(Stats *s);
void del_key(Stats *s);



#endif