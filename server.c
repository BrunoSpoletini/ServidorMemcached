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

int fd_readline(int fd, char *buf)
{
	int rc;
	int i = 0;
	while ((rc = read(fd, buf + i, 1)) > 0)
	{
		if (i > 2048){
			printf("EINVAL");//IMPLEMENTAR
			//einvalFlag = 1;
			// hay que seguir leyendo hasta encontrar el /n 
		}

		//if (buf[i] ) no imprimible -> einval?

		// //fflush( stdin );
		if (buf[i] == '\n'){
			break;
		}
		i++;
	}
	if (rc < 0)
		return -1;

	return i;
}

/*
Casos a considerar:
Se recibe una string sin \n: ("PUT ke")
	- Se guarda en la estructura de eloop_data 
Mientras estamos leyendo:
	Llega un "\n":
		- 
	i > 2048:
		-Se guarda una bandera de einval=true
		-Se sigue leyendo hasta llegar al /n
Cuando terminamos de leer:
	Cuando unimos las strings que nos llegaron al eloop_data:
	Si resulta mas larga que 2048:
		-Respondemos con einval y desechamos la request

*/

int processReq(eloop_data* data, char** req){
		HashTable *table = &hTable; //Temporal hasta q resolvamos la hashtable con colisiones
		int size;
		char clave[2048]; //Esto no va a hacer falta, pq en la version nueva la clave y el valor son strings

		if (!strcmp(req[0], "PUT"))
		{ // PUT test 123
			pthread_mutex_lock(&mutex);
			printf("llamamos a put, con argumentos %s y %d\n",req[1],atoi(req[2]) );
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
		} else {
			return -1;
		}
	return 0;
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

int parseLine(eloop_data* data, char* buff, char* req[3]){
	int words = 0;
	char* token = strtok(buff, " \n");

	for (int i = 0; token != NULL && words <= 3; i++)
	{
		words++;
		req[i] = malloc(strlen(token) + 1);
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
firstLine: si es la primera linea que se lee
*/
void handleConnText(eloop_data* data, int firstLine)
{	
	char buf[READ_SIZE];
	int rc, csock = data->fd;

	/* Atendemos pedidos, uno por linea */
	rc = fd_readline(csock, buf);
	printf("leimos %d caracteres\n",rc);

	if ( rc == -1)
		quit("Fallo al leer");

	if ( rc == 0 ){
		/* Linea vacia, se cerró la conexión */
		write(csock, "Cliente desconectado\n", 21);
		if (epoll_ctl(data->epfd, EPOLL_CTL_DEL, csock, NULL) == -1)
		{
			close(data->epfd);
			quit("Fallo al quitar fd de epoll\n");
		}
		close(csock);
	} else {
		if ( buf[rc] == '\n' ){ //Se recibio una linea completa
			char *req[3];
			if ( parseLine(data, buf, req) == -1 ){
				write(csock, "Comando invalido\n", 17);
			} else {
				if ( processReq(data, req) == -1 ){
					write(csock, "Comando invalido\n", 17);
				}
			}
			data->buffSize = 0;
			handleConnText(data, 0);
		} else {
			// FALTA TESTEAR BIEN
			strcpy(data->buff + data->buffSize, buf );
			data->buffSize += rc;
			//for(int i=0; i<data->buffSize;i++){printf("%c", data->buff[i]);}
			agregarClienteEpoll(csock, data->epfd, 0, (void*)data);
		}
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
				data->isText ? handleConnText(data, 1) : handleConnBin(data);
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