#ifndef __COMM_H
#define __COMM_H 1

#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_EVENTS 256
#define READ_SIZE 256
#define MAX_CLIENTS 256
#define TIMEOUT 100
#define MAX_THREADS 4
#define TEXT_PORT 8888
#define BIN_PORT
enum code {
	PUT = 11,
	DEL = 12,
	GET = 13,

	STATS = 21,

	OK = 101,
	EINVALID = 111,
	ENOTFOUND = 112,
	EBINARY = 113,
	EBIG = 114,
	EUNK = 115,
	EOOM = 116,
};

// Cierra el programa e imprime un mensaje de error
void quit(char *s);

/*
Es lo que dio la catedra, lo vamos descomentando a medida q lo vayamos usando
int valid_rq(int code);

struct eventloop_data {
	int epfd; // file descriptor para epoll
	int id;
	int n_proc;
};

static const in_port_t mc_lport_text = 8888;
static const in_port_t mc_lport_bin  = 8889;

static inline void quit(char *s)
{
	perror(s);
	exit(1);
}

#define STATIC_ASSERT(p)			\
	int _ass_ ## __LINE__ [(!!(p)) - 1];

const char * error_str(enum code e);
*/

#endif