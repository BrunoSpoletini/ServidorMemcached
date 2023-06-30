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

void processReq(eloop_data* data, char** req){
	
	if (!strcmp(req[0], "PUT"))
	{
		Node* nodo = create_node_from_KV(req[1], strlen(req[1]), req[2], strlen(req[2]));
		_PUT(hTable, nodo);
		write(data->fd, "OK\n", 4);
	}
	else if (!strcmp(req[0], "DEL") && req[2] == NULL)
	{
		Node* nodo = create_node_from_K(req[1], strlen(req[1])); 

		int res = _DEL(hTable, nodo);

		if ( res == OK ){
			write(data->fd, "OK\n", 3);
		} else {
			write(data->fd, "ENOTFOUND\n", 10);
		}
		
	}
	else if (!strcmp(req[0], "GET") && req[2] == NULL)
	{	
		Node* nodo = create_node_from_K(req[1], strlen(req[1])); 
		char* res = NULL;
	
		int ret = _GET(hTable, nodo, &res);


		switch ( ret )
		{
		case ENOTFOUND:
			write(data->fd, "ENOTFOUND\n", 10);
			break;
		case EOOM:
			write(data->fd, "EOOM\n", 4);
			break;
		default:

		printf("Salio todo bien: %s\n", res);
			write(data->fd, "OK ", 3);
			write(data->fd, res, strlen(res));
			write(data->fd, "\n", 1);
			break;
		}

	} else if (!strcmp(req[0], "STAT") && req[1] == NULL){
		printf("Stat\n");
		//do stuff
	}
}

int validateReq(char **req, int words){
	if (((strcmp("PUT", req[0]) == 0) && words == 3) ||
		((strcmp("GET", req[0]) == 0) && words == 2) ||
		((strcmp("DEL", req[0]) == 0) && words == 2) ||
		((strcmp("STAT", req[0]) == 0) && words == 1)) {
			return 1;
	} else {
		return 0;
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

int parseLine(eloop_data* data, char* buff, char** req){
	int words = 0;
	char* token = strtok(buff, " \n");
	if(buff[0] == '\0') // Linea vacia
		return -2;
	for (int i = 0; token != NULL && words <= 3; i++)
	{
		words++;
		req[i] = malloc(sizeof(char)*(strlen(token) + 1));
		if (req[i] == NULL)
			quit("Fallo malloc");
		
		strcpy(req[i], token);
		req[i][strlen(token) + 1] = '\0';
		req[i + 1] = NULL;
		token = strtok(NULL, " \n");
	}
	if (words > 0 && words <= 3 && validateReq(req, words))
		return 0;
	return -1;
}

int fd_readline(eloop_data* data)
{
	int ret, conectado=1, i=0, linea = 0;
	char buffer[READ_SIZE+2];
	strcpy(buffer, data->buff);
	int rc = read(data->fd, buffer + data->buffSize, READ_SIZE - (data->buffSize)  );
	//printf("Se llama a readline\n");
	if (rc > 0){
		//printf("Se lee: [");
		//for(int k=0; k<(rc+data->buffSize); k++){printf("%c", buffer[k]);}
		//printf("]\n");
		buffer[data->buffSize + rc] = '\0';
		while ( conectado && (i <= rc) ){ // Recorremos la cadena recibida

			if ( (buffer[data->buffSize+i] < 32 && buffer[data->buffSize+i] != 10 && buffer[data->buffSize+i] != 13 && buffer[data->buffSize+i] != 0) || 126 < buffer[data->buffSize+i]){
				//printf("Caracter no imprimible: %d, en el i; %d\n", buffer[data->buffSize+i], i);
				data->notPrintable = 1;
			}
			if ( buffer[data->buffSize+i] == '\n' ){
				buffer[data->buffSize+i] = '\0';
				if ( data->einval ){
					write(data->fd, "EINVAL\n", 7);
				} else if ( data->notPrintable == 1) {
					write(data->fd, "Comando invalido - Caracteres no imprimible\n", 44);
				} else {
					char **req = malloc(sizeof(char*) * 3);
					//printf("Lo que se le pasa al parser: -%s-\n", buffer+linea);
					ret =  conectado ? parseLine(data, buffer + linea, req) : -2;
					if ( ret == -1 ){
						write(data->fd, "Comando invalido\n", 17);
					} else if ( ret == -2){ //Se desconecta el cliente
						//printf("DESCONECTADO\n");
						conectado = 0;//return -1;
					}else{
						processReq(data, req);
					}
				}
				data->notPrintable = 0;
				data->einval = 0;
				linea = data->buffSize + i + 1;
			}
			
			i++;
		}
		//printf("Linea: %d, RC: %d\n", linea, rc);
		//printf("Copiamos: '%s' en data->buff\n", buffer + linea);
		strcpy(data->buff, buffer + linea);
		data->buffSize = rc + data->buffSize - linea;
		data->buff[data->buffSize] = '\0';

		if ( data->buffSize >= READ_SIZE && linea == 0 ) {
			data->einval = 1;
			data->buffSize = 1;
			data->buff[data->buffSize] = '\0';
		}
	}
	if (!conectado)
		return 0;
	return rc;
}

void handleConnText(eloop_data* data)
{	
	int rc = fd_readline(data);

	if ( rc == -1 )
		quit("Fallo al leer");

	if ( rc == 0){
		desconectarCliente(data);
		printf("Desconectar cliente\n");
	}

	 if ( rc > 0 ){
		agregarClienteEpoll(data->fd, data->epfd, 0, (void*)data);
	} 
}

void handleConnBin(eloop_data* data){
	printf("Bin 10101010\n");
}

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
		events_count = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
		for (int i = 0; i < events_count; i++){
			data = ((eloop_data*)events[i].data.ptr);
			event_fd = data->fd;
			if ( (event_fd == textSock) || (event_fd == binSock) ){
				csock = accept(event_fd, NULL, NULL);
				if (csock == -1){
					quit("Fallo al aceptar un cliente");
				} else {
					agregarClienteEpoll(csock, epoll_fd, (event_fd==binSock) + 1, NULL);
				}
			} else {
				data->isText ? handleConnText(data) : handleConnBin(data);
			}
		}
	}
}

int main(int argc, char **argv){
	hTable =  create_table();
	pthread_t t[MAX_THREADS];
	int textSock, binSock;
	textSock = atoi(argv[1]);
	binSock = atoi(argv[2]); 

	int epoll = create_epoll();
	int threadParam[3] = {epoll, textSock, binSock}; 

	agregarSocketEpoll(textSock, epoll);
	agregarSocketEpoll(binSock, epoll);

	for (int i = 0; i < MAX_THREADS; i++){
		pthread_create(&(t[i]), NULL, wait_for_clients, (void*)threadParam);
	}
	

	// Contemplar opcion de cerrar el servidor
	//destruir_tabla(hTable);
	for (int i = 0; i < MAX_THREADS; i++){
		pthread_join(t[i], NULL);
	}

	return 0;
}