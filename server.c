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
HashTable hTable;

void processReq(eloop_data* data, char** req){
		HashTable *table = &hTable; //Temporal hasta q resolvamos la hashtable con colisiones
		int size;
		char clave[READ_SIZE]; //Esto no va a hacer falta, pq en la version nueva la clave y el valor son strings

		if (!strcmp(req[0], "PUT"))
		{ // PUT test 123
			pthread_mutex_lock(&mutex);
			printf("llamamos a put, con argumentos %s y %s\n",req[1],req[2] );
			insert(table, req[1], atoi(req[2]));
			pthread_mutex_unlock(&mutex);
			write(data->fd, "OK\n", 4);
			/*
			Node* node;
			node = create_node(req[1], req[2], strlen(), )
			
			*/
		}
		else if (!strcmp(req[0], "DEL") && req[2] == NULL)
		{
			pthread_mutex_lock(&mutex);
			printf("llamamos a del, con argumentos %s\n",req[1] );
			delete (table, req[1]);
			pthread_mutex_unlock(&mutex);
			write(data->fd, "Clave-valor eliminado exitosamente\n", 35);
		}
		else if (!strcmp(req[0], "GET") && req[2] == NULL)
		{
			pthread_mutex_lock(&mutex);
			int resGet = get(table, req[1]);
			pthread_mutex_unlock(&mutex);
			if (resGet == -1)
			{
				write(data->fd, "No existe clave para el valor ingresado\n", 40);
			}
			else
			{
				sprintf(clave, "%d", resGet);
				size = strlen(clave);
				write(data->fd, clave, size);
				write(data->fd, "\n", 1);
			}
		} else if (!strcmp(req[0], "STAT") && req[1] == NULL){
			printf("Stat\n");
			//do stuff
		} 
}

int validateReq(char *req[3], int words){
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

int parseLine(eloop_data* data, char* buff, char* req[3]){
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
		req[i + 1] = NULL;
		token = strtok(NULL, " \n");
	}
	if (words > 0 && words <= 3 && validateReq(req, words))
		return 0;
	return -1;
}


/*
Leeemos hasta llegar a un /n, si superamos los READ_SIZE caracteres leyendo el /n seteamos la flag de error EINVAL
En caso de no llegar al /n, si superamos READ_SIZE seteamos flag EINVAL, y si no los superamos, 
revisamos que el caracter que estamos leyendo sea imprimible.
*/
int fd_readline(eloop_data* data, int fd, char *buf, int* errP)
{
	int ret, conectado=1;
	int i=0, linea = 0;
	char buffer[READ_SIZE+2];
	strcpy(buffer, data->buff);
	int rc = read(fd, buffer + data->buffSize, READ_SIZE - (data->buffSize)  );
	printf("Se llama a readline\n");
	if (rc > 0){
		printf("Se lee: [");
		for(int k=0; k<(rc+data->buffSize); k++){printf("%c", buffer[k]);}
		printf("]\n");
		buffer[data->buffSize + rc] = '\0';
		while ( conectado && (i <= rc) ){ // Recorremos la cadena recibida

			if ( (buffer[data->buffSize+i] < 32 && buffer[data->buffSize+i] != 10 && buffer[data->buffSize+i] != 13 && buffer[data->buffSize+i] != 0) || 126 < buffer[data->buffSize+i]){
				printf("Caracter no imprimible: %d, en el i; %d\n", buffer[data->buffSize+i], i);
				data->notPrintable = 1;
			}
			if ( buffer[data->buffSize+i] == '\n' ){
				buffer[data->buffSize+i] = '\0';
				if ( data->einval ){
					write(data->fd, "EINVAL\n", 7);
				} else if ( data->notPrintable == 1) {
					write(data->fd, "Comando invalido - Caracteres no imprimible\n", 44);
				} else {
					char *req[3];
					printf("Lo que se le pasa al parser: -%s-\n", buffer+linea);
					ret =  conectado ? parseLine(data, buffer + linea, req) : -2;
					if ( ret == -1 ){
						write(data->fd, "Comando invalido\n", 17);
					} else if ( ret == -2){ //Se desconecta el cliente
						printf("DESCONECTADO\n");
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
		

		printf("Linea: %d, RC: %d\n", linea, rc);
		//if ((rc - linea) != 0){
		printf("Copiamos: '%s' en data->buff\n", buffer + linea);
		strcpy(data->buff, buffer + linea);
		//} else {
		//	printf("RC antes de terminar readline es: %d\n",rc);
		//	printf("Se leyo todo\n");
		//}
		data->buffSize = rc + data->buffSize - linea;
		data->buff[data->buffSize] = '\0';

		if ( data->buffSize >= READ_SIZE && linea == 0 ) {
			data->einval = 1;
			data->buffSize = 1;
			data->buff[data->buffSize] = '\0';
		}
		
		//printf("Cuando termina de leer: data->buffSize: %d, data->buff: %s\n", data->buffSize, data->buff);
	}
	if (!conectado)
		return -1;

	return rc;
}

void handleConnText(eloop_data* data)
{	
	char buf[READ_SIZE];
	int rc, csock = data->fd;
	int err=0;
	int* errP = &err;
	/* Atendemos pedidos, uno por linea */
	rc = fd_readline(data, csock, buf, errP);

	if ( rc == -2)
		quit("Fallo al leer");

	if ( rc == -1 ){//&& data->buff[data->buffSize] == '\n' ){
		desconectarCliente(data);
		printf("Desconectar cliente\n");
	}

	 if ( rc > 0 ){
		agregarClienteEpoll(csock, data->epfd, 0, (void*)data);
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
					printf("agregamos cliente\n");
					agregarClienteEpoll(csock, epoll_fd, (event_fd==binSock) + 1, NULL);
				}
			} else {
				printf("Pasa por epoll\n");
				data->isText ? handleConnText(data) : handleConnBin(data);
				}
		}
	}
}

//make clean && sudo make run
int main(int argc, char **argv){

	pthread_t t[MAX_THREADS];
	initHashTable(&hTable);
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

	// Necesario?
	for (int i = 0; i < MAX_THREADS; i++){
		pthread_join(t[i], NULL);
	}

	return 0;
}