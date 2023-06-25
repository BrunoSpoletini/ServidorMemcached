#ifndef __SOCK_H
#define __SOCK_H 1

/* Crea un socket de escucha TCP en puerto "port" */
int lsock_tcp(int port);

/* Registra un fd a epoll
 La variable init indica si es la primera vez que se registra el fd */
void agregarClienteEpoll(int fd, int epoll_fd, int init);

/* Crea una instancia de epoll y agrega un socket a la lista de control */
int create_epoll(int sock);

#endif
