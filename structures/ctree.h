#ifndef __ITREE_H__
#define __ITREE_H__

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX(a,b) (((a)>(b))?(a):(b))


typedef struct _CNodo {
  void *dato;
  int altura;
  struct _CNodo *izq;
  struct _CNodo *der;
} CNodo;

typedef CNodo *CTree;

typedef void (*FuncionDestruccion) (void *nodo);

typedef char *(*FuncionObtencion) (void *dato);

typedef int (*FuncionComparacion) (void *dato1, void *dato2);

// Crea un arbol de intervalos vacio.
CTree CTree_crear();

// Destruye un arbol de intervalos.
void ctree_destruir(CTree nodo, FuncionDestruccion liberar);

// Inserta un intervalo en un arbol de intervalos.
CTree ctree_insertar(CTree raiz, void *dato, FuncionComparacion comparar,
                     FuncionDestruccion liberar);

// Devuelve un puntero al dato buscado dentro del arbol dado
void *ctree_buscar(char *dato, CTree nodo, FuncionComparacion comparar,
                   FuncionObtencion obtener_dato);

// Recibe un nodo, y corrige el desbalance de las ramas.
CTree balancear(CTree nodo);

#endif                          /* __ITREE_H__ */
