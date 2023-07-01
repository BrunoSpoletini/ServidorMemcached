#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define BUFF_SIZE 2048
#define N 1
#define q 1


int sock[N];
bool trybind(int i,int aux){


	int rc;

	struct sockaddr_in clientaddr;
	srand (time(NULL));
	int portnum = 10000+aux;//rand() % 10000 + 10000;
	int ipaddrnum = 10+aux;//rand() / 10000 + 10;
	printf("tenemos portnumm = %d, ipaddrun = %d\n",portnum,ipaddrnum );
	char ipaddr[32];

	memset(&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(portnum);
	sprintf(ipaddr, "127.0.0.%d", ipaddrnum);
	inet_pton(AF_INET, ipaddr, &clientaddr.sin_addr);
	rc = -1;
	
	rc = bind(sock[i], (struct sockaddr *)&clientaddr, sizeof(clientaddr));
	if(rc < 0)
		return false;

	return true;
}

int main()
{
	int cto, i;
	struct sockaddr_in servaddr;
	char buffer[BUFF_SIZE], buffer2[BUFF_SIZE], buffer3[BUFF_SIZE];
	struct timespec tsp;

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(888);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	tsp.tv_sec = 0;
	tsp.tv_nsec = 100000;

	for (i = 0; i < N; i++) {
		int rc;

		printf("Conectando cliente %d...\n", i);
		sock[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (sock[i] < 0) {
			perror("CLNT: Error creando socket");
			return -1;
		}

		/*
		 * Esto sólo está para no agotar los puertos efímeros
		 * de la dirección 127.0.0.1, de los cuales hay aprox
		 * 30000. Hacemos un bind() explicito a puertos e IPs
		 * (de origen) distintas a modo de evitarlo. (Todas
		 * las IP 127.x.y.z representan al host local.)
		 */

		int j = i;
		while(!trybind(i,j)){
			j++;
		}
		/*

		{
			struct sockaddr_in clientaddr;
			srand (time(NULL));
			int portnum = 10000+i;//rand() % 10000 + 10000;
			int ipaddrnum = 10+i;//rand() / 10000 + 10;
			printf("tenemos portnumm = %d, ipaddrun = %d\n",portnum,ipaddrnum );
			char ipaddr[32];

			memset(&clientaddr, 0, sizeof(clientaddr));
			clientaddr.sin_family = AF_INET;
			clientaddr.sin_port = htons(portnum);
			sprintf(ipaddr, "127.0.0.%d", ipaddrnum);
			inet_pton(AF_INET, ipaddr, &clientaddr.sin_addr);
			rc = -1;
			printf("llego al while\n");
			while (rc < 0){
				rc = bind(sock[i], (struct sockaddr *)&clientaddr, sizeof(clientaddr));
			}
			printf("paso el while\n");
			if (rc < 0) {
				perror("ECHOCLNT: Error llamando a bind()");
				return -1;
			}
		}
		*/


		rc = connect(sock[i], (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (rc < 0) {
			perror("CLNT: Error conectando");
			return -1;
		}
		
		nanosleep(&tsp, NULL);
	}



	for (i = 0; i < 100; i++) {

		/*
		// Cliente conectado
		cto = read(sock[i], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		printf("%s\n", buffer);
		*/

		int rc = 0;
		sprintf(buffer2,"PUT C%d %d\nAWDS\nGET C%d\nPUT clave val\n", i+430, i+430,i+430);
		cto = strlen(buffer2);

#if q
		rc = write(sock[0], buffer2, cto);
		if (rc < 0) {
			perror("CLNT: Error escribiendo");
			return -1;
		}
#else
		/* Escribe de a un byte, tal vez útil para testear el servidor */
		printf("mandamos bytes\n");
		for (int j = 0; j < cto; j++) {
			rc = write(sock[0], buffer2+j, 1);
			printf("%d - ",j);
			if (rc < 0) {
				perror("CLNT: Error escribiendo");
				return -1;
			}
		}
#endif

		printf("\nllegamos a la parte de las rspuestas\n");

		// ok

		int index = 0;
		int voy = 0;

		while(voy != 4){

			cto = read(sock[0], buffer+index, (sizeof(buffer) - 1) - index);

			if (cto < 0) {
				perror("CLNT: Error leyendo");
				return -1;
			}

			for(int j = index; j < index+cto; j++){
				if(buffer[j] == '\n'){
					//printf("encontre \n en indice %d\n",j);
					buffer[j] = 0;
					printf("respuesta #%d: \n",voy);
					int desde = j-1;
					for(int k = j-1; k >= 0; k--){
						if(buffer[k] == 0){
							break;
						}
						desde = k;
					}
					printf("%s\n", (buffer+desde) );
					voy++;
				}
			}
			index += cto;

		}
		/*

		cto = read(sock[0], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		buffer[cto] = 0;
		printf("%s\n", buffer);

		//comando inv
		printf("segunda\n");
		cto = read(sock[0], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		buffer[cto] = 0;
		printf("%s\n", buffer);

		//Resp
		printf("tercera\n");
		cto = read(sock[0], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		buffer[cto] = 0;
		printf("%s\n", buffer);

		//aux
		printf("cuarta\n");
		cto = read(sock[0], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		buffer[cto] = 0;
		printf("%s\n", buffer);

		*/

		// rc = 0;
		// sprintf(buffer3,"CHAU\n");
		// cto = strlen(buffer3);
		// rc = write(sock[i], buffer3, cto);
		// if (rc < 0) {
		// 	perror("CLNT: Error escribiendo");
		// 	return -1;
		// }
	}

	return 0;
}