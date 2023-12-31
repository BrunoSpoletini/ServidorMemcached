#ifndef __SOCK_H
#define __SOCK_H 1
#include "structures/hash.h"

/* Estructura que almacena los datos de cada fd agregado a epoll */
typedef struct _eloop_data {
	int fd, epfd; 
	int isText, einval, notPrintable;
	char *buff;
	int buffSize;
	int cont, comm;
	int keySize, valueSize;
	char* key;
	char* value;
	Hashtable* hTable;
} eloop_data;

/* Agrega un cliente a la lista de escucha de epoll */
void agregarClienteEpoll(int cliente, int epoll_fd, int init, void *dataPtr, Hashtable* hTable);

/* Registra un socket a la lista de escucha de epoll */
void agregarSocketEpoll(int sock, int epoll_fd, int init);

/* Crea una instancia de epoll */
int create_epoll();

/* Elimina un socket de la instancia de epoll */
void desconectarCliente(eloop_data* data);

#endif
