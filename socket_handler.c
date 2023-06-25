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

	/* Crear socket */
	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock < 0)
		quit("socket");

	/* Setear opción reuseaddr... normalmente no es necesario */
	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bindear al puerto 3942 TCP, en todas las direcciones disponibles */
	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	/* Setear en modo escucha */
	rc = listen(lsock, MAX_CLIENTS);
	if (rc < 0)
		quit("listen");

	return lsock;
}

int create_epoll(int sock)
{
	struct epoll_event ev;
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		quit("Fallo al crear epoll fd\n");
	}
	ev.events = EPOLLIN;
	ev.data.fd = sock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev))
	{
		close(epoll_fd);
		quit("Fallo al agregar fd a epoll\n");
	}
	return epoll_fd;
}

void agregarClienteEpoll(int fd, int epoll_fd, int init){
	struct epoll_event ev;
	int op = init ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
	ev.events = EPOLLIN | EPOLLONESHOT;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, op, fd, &ev))
	{
		close(epoll_fd);
		quit("Fallo al agregar fd a epoll\n");
	}
	if (init)
		write(fd, "Cliente conectado!\n", 19);
}