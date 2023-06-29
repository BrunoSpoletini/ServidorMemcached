#include "dlist.h"
#include <stdlib.h>

DList *dlist_crear() {
  DList *lista = malloc(sizeof(DList));
  lista->primero = NULL;
  lista->ultimo = NULL;
  return lista;
}

void dlist_destruir(void *lista) {
  DNodo *nodoAEliminar;
  while (((DList *) lista)->primero != NULL) {
    nodoAEliminar = ((DList *) lista)->primero;
    ((DList *) lista)->primero = nodoAEliminar->sig;
    free(nodoAEliminar->dato);
    free(nodoAEliminar);
  }
  free(lista);
}

void dlist_agregar_final(DList * lista, void *dato) {
  DNodo *nuevoNodo = malloc(sizeof(DNodo));
  nuevoNodo->dato = dato;
  nuevoNodo->sig = NULL;
  nuevoNodo->ant = lista->ultimo;

  if (lista->ultimo != NULL) {
    lista->ultimo->sig = nuevoNodo;
  }
  if (lista->primero == NULL) {
    lista->primero = nuevoNodo;
  }
  lista->ultimo = nuevoNodo;
}


DNodo *buscar_nodo(DList * lista, void *nodo, FuncionComparativa compare){
  DNodo *aux = lista->primero;
  while(aux != NULL){
    if( compare(aux->dato, nodo) )
        return aux;
  }
  return NULL;
}

void eliminar_nodo(DList * lista, DNodo * nodo, FuncionVisitante liberarDato) {
  if (nodo->ant == NULL)
    lista->primero = nodo->sig;
  else
    nodo->ant->sig = nodo->sig;
  if (nodo->sig == NULL)
    lista->ultimo = nodo->ant;
  else
    nodo->sig->ant = nodo->ant;

  liberarDato(nodo->dato); 
  free(nodo);
}