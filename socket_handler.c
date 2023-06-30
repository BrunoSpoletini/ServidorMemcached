#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/epoll.h>
#include <unistd.h>
#include "common.h"

int lsock_tcp(int port)
{
	struct sockaddr_in sa;
	int lsock;
	int rc;
	int yes = 1;

	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock < 0)
		quit("socket");

	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	rc = listen(lsock, MAX_CLIENTS);
	if (rc < 0)
		quit("listen");

	return lsock;
}

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
		eloop_data *eloop = malloc(sizeof(eloop_data));
		eloop->fd = cliente;
		eloop->epfd = epoll_fd;
		eloop->isText = (init==1);
		if ( eloop->isText ) {
			char* buffer = malloc(sizeof(char)*READ_SIZE + 1);
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
	if(init) printf("Cliente conectado\n"); //DEBUG
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
	write(data->fd, "Cliente desconectado\n", 21);
	if (epoll_ctl(data->epfd, EPOLL_CTL_DEL, data->fd, NULL) == -1)
	{
		close(data->epfd);
		quit("Fallo al quitar fd de epoll\n");
	}
	close(data->fd);
}