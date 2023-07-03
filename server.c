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


void *wait_for_clients(void *threadParam)
{
	int events_count, epoll_fd, csock, textSock, binSock, event_fd;
	eloop_data* data;

	Thread_data *tdata = threadParam;

	epoll_fd = tdata->epollfd;
	textSock = tdata->textSock;
	binSock = tdata->binSock;
	Hashtable *hTable = tdata->hTable;

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
				handleConn(data);
			}
		}
	}
}



int main(int argc, char **argv){

	Hashtable* hTable =  create_table();
	int textSock, binSock,nthreads;
	textSock = atoi(argv[1]);
	binSock = atoi(argv[2]);
	nthreads = atoi(argv[3]);



	int epoll = create_epoll();

	Thread_data *tdata = malloc(sizeof(Thread_data));
	tdata->binSock = binSock;
	tdata->epollfd = epoll;
	tdata->textSock = textSock;
	tdata->hTable = hTable;

	agregarSocketEpoll(textSock, epoll);
	agregarSocketEpoll(binSock, epoll);

	pthread_t t[nthreads];

	for (int i = 0; i < nthreads; i++){
		pthread_create(&(t[i]), NULL, wait_for_clients, (void*)tdata);
	}
	
	for (int i = 0; i < nthreads; i++){
		pthread_join(t[i], NULL);
	}

	return 0;
}