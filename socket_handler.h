#ifndef __SOCK_H
#define __SOCK_H 1
#include "structures/hash.h"
/* Crea un socket de escucha TCP en puerto "port" */
int lsock_tcp(int port);

/* Estructura que almacena los datos de cada fd agregado a epoll */
typedef struct _eloop_data {
	int fd;
	int epfd; // file descriptor de epoll
	int isText;
	char *buff;
	int buffSize;
	int einval;
	int notPrintable;

	int cont; // Se podria usar buffSize como contador para el parser binario, ya q el buff no se usa
	int comm; //Esto se podria usar para el parser de texto tambien
	char* key;
	int keySize;
	char* value;
	int valueSize;
	Hashtable* hTable;
} eloop_data;

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

/* Checkea si el socket esta abierto */
int isSocketUp(int socket_fd);
#endif
