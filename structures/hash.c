#include "hash.h"
#include "dlist.h"
#include "ctree.h"
#include "conjunto.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

int hash_string(char *value) {
  unsigned long long int key = 0;
  for (int i = 0; value[i] != '\0'; i++) {
    key += value[i] * pow(2, (i % 10));
  }
  key = key % TABLESIZE;
  return (int) key;
}

CTree *crear_tabla() {
  CTree *tabla = malloc(sizeof(CTree) * TABLESIZE);
  for (int i = 0; i < TABLESIZE; i++) {
    tabla[i] = NULL;
  }
  return tabla;
}


void insertar_elem_tabla(void *dato, CTree * tabla,
                         FuncionObtencion obtenerCadena,
                         FuncionComparacion dlistComparar, int codigoError) {
  int hash = hash_string(obtenerCadena(dato));
  if (dato != NULL)
    tabla[hash] =
        ctree_insertar(tabla[hash], dato, dlistComparar, dlist_destruir);
  else
    mensaje_error(codigoError);
}

void *buscar_elem_tabla(char *string, CTree * tabla) {
  int hash = hash_string(string);
  return ctree_buscar(string, tabla[hash], comparar_alias, dlist_alias);
}

void liberar_tabla(CTree * tabla) {
  for (int i = 0; i < TABLESIZE; i++) {
    ctree_destruir(tabla[i], dlist_destruir);
  }
  free(tabla);
}


int PUT(Hashtable *ht, Node *node){

    int index = node->hash;
    lock( &ht->Tlock[index] );

    insertar_elem_tabla( node, ht->tree[index], equal_keys);

    unlock(&ht->Tlock[Node->hash]);

    return OK;
}


char *copy(char *s, int len){

  char *c = tryalloc(len);
  if(c == EOOM){
    return EOOM;
  }

  strcpy(c,s);
  
  return c;
}


void* GET(Node *node){ /// podemos usar un node vacio, que solo contiene la key y el lenkey (total son las unicas dos cosas que se usan al comparar).

  int index = node->hash;

  lock( &ht->Tlock[index] );

  Node *elem = buscar_elem_tabla(node,ht->tree[index]);
  if(elem == NULL){
    destroy_node(node); // capaz esto se puede hacer afuera? para consumir menos el lock.
    return ENOTFOUND;
  }

  char* retval = copy(elem->value, elem->lenvalue); /// copiamos por si alguien mas la edita / elimina en el medio.

  unlock(&ht->Tlock[Node->hash]);

  return retval;
}


void *evict(Hashtable *ht, unsigned bytes){
  /// tenemos que liberar bytes de la ht, soltando por la politica de desalojo.

}


int DEL(Node *node){ /// podemos usar un node vacio, que solo contiene la key y el lenkey (total son las unicas dos cosas que se usan al comparar).

  int index = node->hash;

  lock( &ht->Tlock[index] );

  Node *elem = buscar_elem_tabla(node,ht->tree[index]);
  if(elem == NULL){
    destroy_node(node); // capaz esto se puede hacer afuera? para consumir menos el lock.
    return ENOTFOUND;
  }

  ctree_eliminar(node,ht->tree[index]);

  unlock(&ht->Tlock[Node->hash]);

  return OK;
}



