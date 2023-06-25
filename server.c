#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>	   // Para hacer un socket no bloqueante
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <sys/wait.h>  //Para usar wait
#include <string.h>	   // strtok
#include <pthread.h>
#include "hash_table.h"
#include "socket_handler.h"
#include "common.h"

/*
Para inicializar la hash table:
	HashTable table;
	initHashTable(&table);

	insert(&table, "apple", 1);

 * Para probar, usar netcat. Ej:
 *      $ nc localhost 3942

Posibles mejoras:
Hacer un sistema de salida, tanto para el cliente como para el serivdor.
Para esto se puede usar pthread_exit(0);

Checkeo de errores? Revisar

Comentado de codigo

Borrar comentarios de ayuda

Mejorar el parseo del input

Unificar todo a camelCase

Arreglar el mutex del get

El delete deberia devolver true o false si borro o no la clave

 */

int lsock;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

HashTable hTable;

int fd_readline(int fd, char *buf)
{
	int rc;
	int i = 0;
	while ((rc = read(fd, buf + i, 1)) > 0)
	{
		//fflush( stdin );
		if (buf[i] == '\n')
			break;
		i++;
	}
	if (rc < 0)
		return rc;

	buf[i] = 0;
	return i;
}

int handle_conn(int csock)
{
	HashTable *table = &hTable;
	char buf[READ_SIZE], *token, **args, clave[100];
	int rc, size;

	/* Atendemos pedidos, uno por linea */
	rc = fd_readline(csock, buf);
	printf("leimos %d caracteres\n",rc);
	if (rc < 0)
		quit("Fallo al leer");

	// Parseo del input
	args = malloc(sizeof(char *) * 3);
	if (args == NULL)
		quit("Fallo malloc");
	token = strtok(buf, " \n"); /// separa segun espacio y \n
	int palabras = 0;
	for (int i = 0; token != NULL; i++)
	{
		args[i] = malloc(strlen(token) + 1);
		if (args[i] == NULL)
			quit("Fallo malloc");
		strcpy(args[i], token);

		token = strtok(NULL, " \n");
		args[i + 1] = NULL;
		palabras++;
	}

		/* Linea vacia, se cerró la conexión */
	if (rc == 0)
	{
		write(csock, "Cliente desconectado\n", 21);
		return -1;
	}

	if (!strcmp(args[0], "PUT") && palabras == 3)
	{ // PUT test 123
		pthread_mutex_lock(&mutex);
		printf("llamamos a put, con argumentos %s y %d\n",args[1],atoi(args[2]) );
		insert(table, args[1], atoi(args[2]));
		pthread_mutex_unlock(&mutex);
		write(csock, "OK\n", 4);
	}
	else if (!strcmp(args[0], "DEL") && palabras == 2)
	{
		pthread_mutex_lock(&mutex);
		printf("llamamos a del, con argumentos %s\n",args[1] );
		delete (table, args[1]);
		pthread_mutex_unlock(&mutex);
		write(csock, "Clave-valor eliminado exitosamente\n", 35);
	}
	else if (!strcmp(args[0], "GET") && palabras == 2)
	{
		pthread_mutex_lock(&mutex);
		int resGet = get(table, args[1]);
		pthread_mutex_unlock(&mutex);
		if (resGet == -1)
		printf("llamamos a get, con argumentos %s\n",args[1] );
		if (get(table, args[1]) == -1)
		{
			write(csock, "No existe clave para el valor ingresado\n", 40);
		}
		else
		{
			sprintf(clave, "%d", resGet);
			size = strlen(clave);
			write(csock, clave, size);
			write(csock, "\n", 1);
		}
		
	}
	return 0;
}



void *wait_for_clients(void *epoll)
{
	int events_count, epoll_fd = *(int*)epoll, csock;
	struct epoll_event events[MAX_EVENTS];

	while (1)
	{
		events_count = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
		for (int i = 0; i < events_count; i++){
			if (events[i].data.fd != lsock){
				csock = events[i].data.fd;
				if (handle_conn(csock) == -1)
				{
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, csock, NULL) == -1)
					{
						close(epoll_fd);
						quit("Fallo al quitar fd de epoll\n");
					}
					close(csock);
				}else{
					agregarClienteEpoll(csock,epoll_fd, 0);
				}
			} else{
				csock = accept(lsock, NULL, NULL);
				if (csock == -1){
					quit("Fallo al aceptar un cliente");
				} else {
					agregarClienteEpoll(csock, epoll_fd, 1);
				}
			}
		}
	}
}





int main(){
	pthread_t t[MAX_THREADS];
	initHashTable(&hTable);

	lsock = lsock_tcp(TEXT_PORT); //Temporal (hasta tener el ejecutable de los permisos)
	int epoll = create_epoll(lsock);

	for (int i = 0; i < MAX_THREADS; i++){
		pthread_create(&(t[i]), NULL, wait_for_clients, (void*)&epoll);
	}

	for (int i = 0; i < MAX_THREADS; i++){
		pthread_join(t[i], NULL);
	}
	return 0;
}