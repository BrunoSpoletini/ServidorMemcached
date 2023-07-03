#ifndef __COMM_H
#define __COMM_H 1

#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"

#define READ_SIZE 2048
#define MSG_SIZE 2048

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

typedef struct{
	int epollfd, textSock,binSock;
	Hashtable *hTable;
}Thread_data;

 typedef struct Hashtable Hashtable;
 typedef struct DList DList;
 typedef struct DNodo DNodo;
 typedef struct Stats Stats;
 typedef struct Node Node;


// Cierra el programa e imprime un mensaje de error
void quit(char *s);

// Convierte una string de un comando en su respectivo codigo
int str_to_comm(char* str);

// Convierte un comando a su respectiva string
const char *code_str(enum code e);

// Compara dos arrays de caracteres del mismo largo
int char_arr_cmp(char* str1, char* str2, int l);

// Compara dos arrays de caracteres del mismo largo
void char_arr_cpy(char* str1, char* str2, int l);


#endif