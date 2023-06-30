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

void dummyfree(void* data){

}

bool evict(Hashtable *ht){

  pthread_mutex_lock( &ht->locklru );

  if(ht->LRU->primero == NULL ){
    pthread_mutex_unlock( &ht->locklru );
    return false;
  }

  DNodo* node = ht->LRU->primero;

  while(node != ht->LRU->ultimo){

    Node* data = (node->othernode->dato);

    if( pthread_mutex_trylock( &ht->rlock[data->hash]) == 0 ){

      dlist_eliminar_nodo(ht->row[data->hash], node->othernode, destroy_node);
      dlist_eliminar_nodo(ht->LRU, node, dummyfree); /// el nodo de la LRU.


      pthread_mutex_unlock( &ht->rlock[data->hash] );
      pthread_mutex_unlock( &ht->locklru );
      return true;
    }

    node = node->sig;
  }


  pthread_mutex_unlock( &ht->locklru );


  return false;
}

void *tryalloc(Hashtable *ht, unsigned bytes){

    void *ret;
    while( (ret = malloc(bytes)) == NULL ){
      if( !evict(ht) )
        return NULL;
    }

    return ret;
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
    add_put(ht->stats);

    int index = node->hash;
    pthread_mutex_lock( &ht->rlock[index] );

    DNodo* elem = dlist_buscar_nodo(ht->row[index], node, equal_keys);

    if(elem == NULL){ // no esta en la lista.

        DNodo* newnode = dlist_crear_nodo(ht,node); /// no copiamos el valor, lo tomamos como puntero.
        if(newnode == NULL){ /// si no pudimos crear el nodo:
          pthread_mutex_unlock( &ht->rlock[index] );
          destroy_node(node);
          return EOOM;
        }

        DNodo* newnodeLRU = dlist_crear_nodo(ht,NULL);

         if(newnodeLRU == NULL){ /// si no pudimos crear el nodo:
          pthread_mutex_unlock( &ht->rlock[index] );
          
          dlist_destruir_nodo(newnode,destroy_node);

          return EOOM;
        }


        newnode->othernode = newnodeLRU; /// linkeamos los nodos entre si.
        newnodeLRU->othernode = newnode;

        dlist_agregar_final( ht->row[index], newnode);

        pthread_mutex_lock( &ht->locklru );
          dlist_agregar_final( ht->LRU, newnodeLRU);
        pthread_mutex_unlock( &ht->locklru );

      pthread_mutex_unlock(&ht->rlock[index]);

      add_key(ht->stats);

    }else{/// ya existe la clave:

        Node *trash = elem->dato;
        elem->dato = node;
        
        updateLRU(ht,elem);

      pthread_mutex_unlock(&ht->rlock[index]);

      destroy_node(trash); /// fuera de la zona critica.

    }

      return OK;
}


int _GET(Hashtable *ht, Node *node, char* retval){ /// podemos usar un node vacio, que solo contiene la key y el lenkey (total son las unicas dos cosas que se usan al comparar).
  add_get(ht->stats);

  int index = node->hash;

  pthread_mutex_lock( &ht->rlock[index] );

  DNodo *elem = dlist_buscar_nodo(ht->row[index], node, equal_keys);
  
  //destroy_node(node); // capaz esto se puede hacer afuera? para consumir menos el lock.

  if(elem == NULL)
    return ENOTFOUND;//(void*)ENOTFOUND; DEBUG

  Node* data = elem->dato;
  retval = copycat(ht, data->value , data->lenvalue); /// copiamos por si alguien mas la edita / elimina en el medio.

  if(retval == NULL)
    return EOOM;//(void*)EOOM;

  updateLRU(ht,elem->othernode);

  pthread_mutex_unlock(&ht->rlock[index]);

  destroy_node(node); // fuera de la zona critica.

  return OK;
}




int _DEL(Hashtable *ht,Node *node){ /// podemos usar un node vacio, que solo contiene la key y el lenkey (total son las unicas dos cosas que se usan al comparar).
  add_del(ht->stats);

  int index = node->hash;

  pthread_mutex_lock( &ht->rlock[index] );

  DNodo *elem = dlist_buscar_nodo(ht->row[index], node, equal_keys);
  
  
  if(elem == NULL)
    return ENOTFOUND;

  
  dlist_eliminar_nodo(ht->LRU, elem->othernode, dummyfree); /// el nodo de la LRU.
  dlist_eliminar_nodo(ht->row[index], elem, destroy_node);

  pthread_mutex_unlock(&ht->rlock[index]);

  destroy_node(node); // fuera de la zona critica.

  del_key(ht->stats);

  return OK;
}










