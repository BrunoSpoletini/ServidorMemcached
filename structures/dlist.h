#ifndef __DLIST_H__
#define __DLIST_H__

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "hash.h"

typedef struct _DNodo {
  void *dato;
  struct _DNodo *ant;
  struct _DNodo *sig;
  struct _DNodo *othernode;
} DNodo;

typedef struct {
  DNodo *primero;
  DNodo *ultimo;
} DList;

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

DNodo *dlist_buscar_nodo(DList * lista, void *nodo, FuncionComparativa compare);

DNodo *dlist_crear_nodo(void *dato);


void dlist_deslinkear(DList * lista, DNodo * nodo);

void dlist_destruir_nodo(DNodo * nodo, FuncionVisitante liberarDato);

#endif                          /* __DLIST_H__ */
