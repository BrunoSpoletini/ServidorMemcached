#ifndef __SOCK_H
#define __SOCK_H 1
#include "structures/hash.h"
/* Crea un socket de escucha TCP en puerto "port" */
int lsock_tcp(int port);

/* Registra un cliente a epoll
init -> 0 no es la primera vez que se registra el cliente
        1 se registra con protocolo texto
        2 se registra con protocolo binario*/

//dataPtr es un puntero a la estructura de datos del evento

void agregarClienteEpoll(int cliente, int epoll_fd, int init, void *dataPtr, Hashtable* hTable);

/* Registra un socket a epoll */
void agregarSocketEpoll(int sock, int epoll_fd);

/* Crea una instancia de epoll */
int create_epoll();

/* Elimina un socket de la instancia de epoll */
void desconectarCliente(eloop_data* data);

#endif
