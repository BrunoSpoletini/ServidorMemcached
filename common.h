#ifndef __COMM_H
#define __COMM_H 1

#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_EVENTS 256
#define READ_SIZE 2048
#define MAX_CLIENTS 256
#define TIMEOUT 100
#define MAX_THREADS 4
#define MSG_SIZE 2048
#define DEFAULT_TEXT_SOCKET 888
#define DEFAULT_BIN_SOCKET 889
#define DEFAULT_MEM_LIMIT 1000000000  ///  1GB
#define TABLESIZE 50000

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

/* Estructura que almacena los datos de cada fd agregado a epoll */
typedef struct _eloop_data {
	int fd;
	int epfd; // file descriptor de epoll
	int isText;
	char *buff;
	int buffSize;
	int einval;
	int notPrintable;
} eloop_data;

/*
Es lo que dio la catedra, lo vamos descomentando a medida q lo vayamos usando
int valid_rq(int code);



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