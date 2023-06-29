#include "dlist.h"
#include <stdlib.h>

DList *dlist_crear() {
  DList *lista = malloc(sizeof(DList));
  lista->primero = NULL;
  lista->ultimo = NULL;
  return lista;
}

DNodo *dlist_crear_nodo(void *dato){
  DNodo *nuevoNodo = tryalloc( sizeof(DNodo) );
  
  if(nuevonodo == NULL)
    return NULL;

  nuevoNodo->dato = dato;
  nuevoNodo->sig = NULL;
  nuevoNodo->ant = NULL;
  nuevoNodo->othernode = NULL;
  return nuevoNodo;
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




void dlist_agregar_final(DList * lista, DNodo *nuevoNodo) {
  nuevoNodo->ant = lista->ultimo;

  if (lista->ultimo != NULL) {
    lista->ultimo->sig = nuevoNodo;
  }
  if (lista->primero == NULL) {
    lista->primero = nuevoNodo;
  }
  lista->ultimo = nuevoNodo;
}


DNodo *dlist_buscar_nodo(DList * lista, void *nodo, FuncionComparativa compare){

  DNodo *aux = lista->primero;
  while(aux != NULL){
    if( compare(aux->dato, nodo) )
        return aux;
  }
  return NULL;
}

void dlist_destruir_nodo(DNodo * nodo, FuncionVisitante liberarDato){
  liberarDato(nodo->dato); 
  free(nodo);
}


void dlist_deslinkear(DList * lista, DNodo * nodo){
  if (nodo->ant == NULL)
    lista->primero = nodo->sig;
  else
    nodo->ant->sig = nodo->sig;
  if (nodo->sig == NULL)
    lista->ultimo = nodo->ant;
  else
    nodo->sig->ant = nodo->ant;
}

void dlist_eliminar_nodo(DList * lista, DNodo * nodo, FuncionVisitante liberarDato) {

  dlist_deslinkear(lista,nodo);
  dlist_destruir_nodo(nodo,liberarDato);

}