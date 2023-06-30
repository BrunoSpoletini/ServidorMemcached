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
#define BUFF_SIZE 2048
#define N 1

int main()
{
	int sock[N], cto, i;
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
		{
			struct sockaddr_in clientaddr;
			srand (time(NULL));
			int portnum = rand() % 10000 + 10000;
			int ipaddrnum = rand() / 10000 + 10;
			char ipaddr[32];

			memset(&clientaddr, 0, sizeof(clientaddr));
			clientaddr.sin_family = AF_INET;
			clientaddr.sin_port = htons(portnum);
			sprintf(ipaddr, "127.0.0.%d", ipaddrnum);
			inet_pton(AF_INET, ipaddr, &clientaddr.sin_addr);
			rc = -1;
			while (rc < 0){
				rc = bind(sock[i], (struct sockaddr *)&clientaddr, sizeof(clientaddr));
			}
			if (rc < 0) {
				perror("ECHOCLNT: Error llamando a bind()");
				return -1;
			}
		}

		rc = connect(sock[i], (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (rc < 0) {
			perror("CLNT: Error conectando");
			return -1;
		}

		nanosleep(&tsp, NULL);
	}

	for (i = 0; i < N; i++) {

		// // Cliente conectado
		// cto = read(sock[i], buffer, sizeof(buffer) - 1);
		// if (cto < 0) {
		// 	perror("CLNT: Error leyendo");
		// 	return -1;
		// }
		// printf("%s\n", buffer);

		int rc = 0;
		sprintf(buffer2,"PUT C%d %d\nAWDS\nGET C430\nPUT clave val", i+430, i+430);
		cto = strlen(buffer2);

#if q
		rc = write(sock[i], buffer2, cto);
		if (rc < 0) {
			perror("CLNT: Error escribiendo");
			return -1;
		}
#else
		/* Escribe de a un byte, tal vez útil para testear el servidor */
		for (int j = 0; j < cto; j++) {
			rc = write(sock[i], buffer2+j, 1);
			printf("%d - ",j);
			if (rc < 0) {
				perror("CLNT: Error escribiendo");
				return -1;
			}
		}
#endif

		// ok
		cto = read(sock[i], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		buffer[cto] = 0;
		printf("%s\n", buffer);

		//comando inv
		cto = read(sock[i], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		buffer[cto] = 0;
		printf("%s\n", buffer);

		//Resp
		cto = read(sock[i], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		buffer[cto] = 0;
		printf("%s\n", buffer);

		//aux
		cto = read(sock[i], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}
		buffer[cto] = 0;
		printf("%s\n", buffer);


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