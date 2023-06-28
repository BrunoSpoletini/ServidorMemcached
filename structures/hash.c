#include "hash.h"


int hash_string(char *value) {
  unsigned long long int key = 0;
  for (int i = 0; value[i] != '\0'; i++) {
    key += value[i] * pow(2, (i % 10));
  }
  key = key % TABLESIZE;
  return (int) key;
}

void inicializar_tabla(Hashtable *ht) {
  
  for (int i = 0; i < TABLESIZE; i++) {
    ht->row[i] = dlist_crear();
    pthread_mutex_init(&ht->rlock[i], NULL); 
  }
  ht->stats = create_stats();

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



int _PUT(Hashtable *ht, Node *node){

    int index = node->hash;
    pthread_mutex_lock( &ht->rlock[index] );

    dlist_agregar_final( ht->row[index], node);

    /// y agregar al lru.

    pthread_mutex_unlock(&ht->rlock[index]);

    return OK;
}




void* _GET(Hashtable *ht, Node *node){ /// podemos usar un node vacio, que solo contiene la key y el lenkey (total son las unicas dos cosas que se usan al comparar).

  int index = node->hash;

  pthread_mutex_lock( &ht->rlock[index] );


  DNodo *elem = buscar_nodo(ht->row[index], node, equal_keys);
  
  destroy_node(node); // capaz esto se puede hacer afuera? para consumir menos el lock.

  if(elem == NULL)
    return (void*)ENOTFOUND;

  /// y agregar al lru.

  char* retval = copycat(ht, ((Node*)elem->dato)->value , ((Node*)elem->dato)->lenvalue); /// copiamos por si alguien mas la edita / elimina en el medio.

  if(retval == NULL)
    return (void*)EOOM;

  pthread_mutex_unlock(&ht->rlock[index]);

  return retval;
}



int _DEL(Hashtable *ht,Node *node){ /// podemos usar un node vacio, que solo contiene la key y el lenkey (total son las unicas dos cosas que se usan al comparar).

  int index = node->hash;

  pthread_mutex_lock( &ht->rlock[index] );

  DNodo *elem = buscar_nodo(ht->row[index], node, equal_keys);
  
  destroy_node(node); // capaz esto se puede hacer afuera? para consumir menos el lock.
  
  if(elem == NULL)
    return ENOTFOUND;

  eliminar_nodo(ht->row[index], elem, destroy_node);

  pthread_mutex_unlock(&ht->rlock[index]);

  return OK;
}










