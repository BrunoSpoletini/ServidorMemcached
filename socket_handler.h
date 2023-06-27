#ifndef __SOCK_H
#define __SOCK_H 1

/* Crea un socket de escucha TCP en puerto "port" */
int lsock_tcp(int port);

/* Registra un cliente a epoll
init -> 0 no es la primera vez que se registra el cliente
        1 se registra con protocolo texto
        2 se registra con protocolo binario*/

//dataPtr es un puntero a la estructura de datos del evento

void agregarClienteEpoll(int cliente, int epoll_fd, int init, void *dataPtr);

/* Registra un socket a epoll */
void agregarSocketEpoll(int sock, int epoll_fd);

/* Crea una instancia de epoll */
int create_epoll();

#endif
