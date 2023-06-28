#ifndef __DLIST_H__
#define __DLIST_H__

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct _DNodo {
  void *dato;
  struct _DNodo *ant;
  struct _DNodo *sig;
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
void dlist_agregar_final(DList * lista, void *dato);


// Dada una lista y un nodo, elimina el nodo de la lista y libera la memoria
void eliminar_nodo(DList * lista, DNodo * nodo, FuncionVisitante liberarDato);


DNodo *buscar_nodo(DList * lista, void *nodo, FuncionComparativa compare);

#endif                          /* __DLIST_H__ */
