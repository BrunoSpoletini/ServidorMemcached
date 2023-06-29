#include <math.h>
#include "hash.h"

Hashtable *create_table() {
  
  Hashtable *ht = malloc(sizeof(Hashtable));

  for (int i = 0; i < TABLESIZE; i++) {
    ht->row[i] = dlist_crear();
    pthread_mutex_init(&ht->rlock[i], NULL); 
  }
    ht->LRU = dlist_crear();
    pthread_mutex_init(&ht->locklru, NULL);

  ht->stats = create_stats();

  return ht;
}

int hash_string(char *value) {
  unsigned long long int key = 0;
  for (int i = 0; value[i] != '\0'; i++) {
    key += value[i] * pow(2, (i % 10));
  }
  key = key % TABLESIZE;
  return (int) key;
}

void *evict(Hashtable *ht, unsigned bytes){
  /// tenemos que liberar bytes de la ht, soltando por la politica de desalojo.
  return NULL;
}

void *tryalloc(Hashtable *ht, unsigned bytes){

    void* space = malloc(bytes);
    if(space == NULL){
         return evict(ht,bytes);
    }

    return space;
}

char *copycat(Hashtable *ht,char *s, int len){
  char *c = tryalloc(ht , len);
  if(c == NULL){
    return NULL;
  }
  strcpy(c,s);
  return c;
}


void updateLRU(Hashtable *ht, DNodo* elem){

  pthread_mutex_lock( &ht->locklru );

    dlist_deslinkear(ht->LRU, elem);
    dlist_agregar_final( ht->LRU, elem);

  pthread_mutex_unlock( &ht->locklru );
}

int _PUT(Hashtable *ht, Node *node){

    int index = node->hash;
    pthread_mutex_lock( &ht->rlock[index] );

    dlist_agregar_final( ht->row[index], node);

    /// y agregar al lru.

      pthread_mutex_unlock(&ht->rlock[index]);

    }else{/// ya existe la clave:

        Node *trash = elem->dato;
        elem->dato = node;
        
        updateLRU(ht,elem);

      pthread_mutex_unlock(&ht->rlock[index]);

      destroy_node(trash); /// fuera de la zona critica.

    }

      return OK;
}


void* _GET(Hashtable *ht, Node *node){ /// podemos usar un node vacio, que solo contiene la key y el lenkey (total son las unicas dos cosas que se usan al comparar).

  int index = node->hash;

  pthread_mutex_lock( &ht->rlock[index] );

  DNodo *elem = dlist_buscar_nodo(ht->row[index], node, equal_keys);
  
  //destroy_node(node); // capaz esto se puede hacer afuera? para consumir menos el lock.

  if(elem == NULL)
    return NULL;//(void*)ENOTFOUND; DEBUG

  Node* data = elem->dato;
  char* retval = copycat(ht, data->value , data->lenvalue); /// copiamos por si alguien mas la edita / elimina en el medio.

  if(retval == NULL)
    return NULL;//(void*)EOOM;

  updateLRU(ht,elem->othernode);

  pthread_mutex_unlock(&ht->rlock[index]);

  destroy_node(node); // fuera de la zona critica.

  return retval;
}



int _DEL(Hashtable *ht,Node *node){ /// podemos usar un node vacio, que solo contiene la key y el lenkey (total son las unicas dos cosas que se usan al comparar).

  int index = node->hash;

  pthread_mutex_lock( &ht->rlock[index] );

  DNodo *elem = dlist_buscar_nodo(ht->row[index], node, equal_keys);
  
  
  if(elem == NULL)
    return ENOTFOUND;

  
  dlist_eliminar_nodo(ht->LRU, elem->othernode, free); /// el nodo de la LRU.
  dlist_eliminar_nodo(ht->row[index], elem, destroy_node);

  pthread_mutex_unlock(&ht->rlock[index]);

  destroy_node(node); // fuera de la zona critica.

  return OK;
}










