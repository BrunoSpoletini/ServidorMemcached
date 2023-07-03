#ifndef __DLIST_H__
#define __DLIST_H__

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../common.h"
#include "hash.h"

 struct DNodo {
  Node *dato;
  struct DNodo *ant;
  struct DNodo *sig;
  struct DNodo *othernode;
} ;

struct DList{
  DNodo *primero;
  DNodo *ultimo;
};


typedef void (*FuncionVisitante) (void *dato);

typedef bool (*FuncionComparativa) (void *dato1, void *dato2);

//  Devuelve una lista vacía.
DList *dlist_crear();

//  Destruccion de la lista.
void dlist_destruir(void *lista);

//  Agrega un elemento al final de la lista.
void dlist_agregar_final(DList * lista, DNodo *nuevoNodo);


// Dada una lista y un nodo, elimina el nodo de la lista y libera la memoria
void dlist_eliminar_nodo(DList * lista, DNodo * nodo, FuncionVisitante liberarDato);

// compara los nodos de la lista segun la funcion compare, y devuelve el primero que matchea.
DNodo *dlist_buscar_nodo(DList * lista, void *nodo, FuncionComparativa compare);

// crea un nodo, pidiendo memoria a la tabla hash de ser necesario.
DNodo *dlist_crear_nodo(Hashtable *ht , Node* dato);

//deslinkea (no libera memoria) un nodo de una lista
void dlist_deslinkear(DList * lista, DNodo * nodo);

//Destruye un nodo
void dlist_destruir_nodo(DNodo * nodo, FuncionVisitante liberarDato);

#endif                          /* __DLIST_H__ */
