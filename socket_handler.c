#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/epoll.h>
#include <unistd.h>
#include "common.h"
#include "socket_handler.h"

int create_epoll()
{
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		quit("Fallo al crear epoll fd\n");
	}
	return epoll_fd;
}

void agregarClienteEpoll(int cliente, int epoll_fd, int init, void *dataPtr, Hashtable* hTable){
	struct epoll_event ev;
	int op = init ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
	ev.events = EPOLLIN | EPOLLONESHOT;

	if(init){ //Si es el primer registro, cargamos la estructura
		eloop_data *eloop = tryalloc(hTable, sizeof(eloop_data));

		if(eloop == NULL)
			return; /// dejamos pasar el cliente.

		eloop->fd = cliente;
		eloop->epfd = epoll_fd;
		eloop->isText = (init==1);
		eloop->key = NULL;
		eloop->value = NULL;
		eloop->buff = NULL;
		if ( eloop->isText ) {
			char* buffer = tryalloc(hTable,(sizeof(char)*READ_SIZE + 1) );
			
			if(buffer == NULL){
				free(eloop);
				return;
			}

			eloop->buff = buffer;
			eloop->buffSize = 0;
		} else {
			eloop->cont = 0;
			eloop->keySize = 0;
			eloop->valueSize = 0;
		}
		eloop->einval = 0;
	    eloop->notPrintable = 0;
		eloop->hTable = hTable;
		ev.data.ptr = eloop;
	} else{ //Caso contrario, recuperamos los datos
		ev.data.ptr = dataPtr;
	}

	if (epoll_ctl(epoll_fd, op, cliente, &ev))
	{
		close(epoll_fd);
		quit("Fallo al agregar cliente a epoll\n");
	}
}

void agregarSocketEpoll(int sock, int epoll_fd){
	struct epoll_event ev;
	ev.events = EPOLLIN;
	eloop_data *eloop = malloc(sizeof(eloop_data));
	ev.data.ptr = eloop;
	((eloop_data*)ev.data.ptr)->fd = sock;
	((eloop_data*)ev.data.ptr)->epfd = epoll_fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev))
	{
		close(epoll_fd);
		quit("Fallo al agregar fd a epoll\n");
	}
}

void desconectarCliente(eloop_data* data){
	if ( data ){
		if(data->buff != NULL){
			free(data->buff);
		}

		if(data->value != NULL){
			free(data->value);	
		}
		if(data->key != NULL){
			free(data->key);
		}
	}
	close(data->fd);
	free(data);
}