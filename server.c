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
#include "structures/hash.h"
#include "structures/Node.h"
#include "structures/stats.h"
#include "structures/dlist.h"
#include "common.h"
#include "socket_handler.h"
#include "utils.h"
#include "reqHandler.h"

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


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//echo -n "\n" | echo -n "a" | echo -n " " | echo -n "T" | echo -n "U" | echo -n "P" | nc localhost 888

Hashtable* hTable;

void *wait_for_clients(void *threadParam)
{
	int events_count, epoll_fd, csock, textSock, binSock, event_fd;
	eloop_data* data;
	epoll_fd = ((int*)threadParam)[0];
	textSock = ((int*)threadParam)[1];
	binSock = ((int*)threadParam)[2];
	struct epoll_event events[MAX_EVENTS];
	while (1)
	{
		printf("soy un thread esperando data\n");
		events_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		printf("soy un thread que paso\n");
		for (int i = 0; i < events_count; i++){
			data = ((eloop_data*)events[i].data.ptr);
			event_fd = data->fd;
			if ( (event_fd == textSock)|| (event_fd == binSock) ){
				csock = accept(event_fd, NULL, NULL);
				if (csock == -1){
					quit("Fallo al aceptar un cliente");
				} else {
					agregarClienteEpoll(csock, epoll_fd, (event_fd==binSock) + 1, NULL, hTable);
				}
			} else {
				printf("EPOLL\n");
				//if ( isSocketUp(event_fd) ){
					handleConn(data);
				//} else {
				//	printf("DESCONECTADO LINCE\n");
				//	desconectarCliente(data);
				//}
			}
		}
	}
}



int main(int argc, char **argv){

	hTable =  create_table();
	int textSock, binSock;
	textSock = atoi(argv[1]);
	binSock = atoi(argv[2]); 

	int epoll = create_epoll();
	int threadParam[3] = {epoll, textSock, binSock}; 

	agregarSocketEpoll(textSock, epoll);
	agregarSocketEpoll(binSock, epoll);


	long Nprocessors = sysconf(_SC_NPROCESSORS_ONLN);
	
	pthread_t t[Nprocessors];

	for (int i = 0; i < Nprocessors; i++){
		pthread_create(&(t[i]), NULL, wait_for_clients, (void*)threadParam);
	}
	

	// Contemplar opcion de cerrar el servidor
	//destruir_tabla(hTable);
	for (int i = 0; i < Nprocessors; i++){
		pthread_join(t[i], NULL);
	}

	return 0;
}