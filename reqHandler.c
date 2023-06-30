#include "reqHandler.h"

/* Common */

void handleConn(eloop_data* data)
{	
	int rc = data->isText? fd_readline_texto(data) : fd_readline_bin(data);

	if ( rc == -1 )
		quit("Fallo al leer");

	if ( rc == 0){
		desconectarCliente(data);
	}

	 if ( rc > 0 ){
		agregarClienteEpoll(data->fd, data->epfd, 0, (void*)data, data->hTable);
	} 
}

void processReq(eloop_data* data, char** req){
	
	if (!strcmp(req[0], "PUT"))
	{
		Node* nodo = create_node_from_KV(req[1], strlen(req[1]), req[2], strlen(req[2]));
		_PUT(data->hTable, nodo);
		write(data->fd, "OK\n", 4);
	}
	else if (!strcmp(req[0], "DEL") && req[2] == NULL)
	{
		Node* nodo = create_node_from_K(req[1], strlen(req[1])); 

		int res = _DEL(data->hTable, nodo);

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
	
		int ret = _GET(data->hTable, nodo, &res);

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

/* Handler texto */



int parseLineText(eloop_data* data, char* buff, char** req){
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

int fd_readline_texto(eloop_data* data)
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
					ret =  conectado ? parseLineText(data, buffer + linea, req) : -2;
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


/* Handler binario */

int etapaBin(int val, eloop_data* data){
	if ( val < 1)
		return 0;
	if ( val < 5 )
		return 1;
	if (val == 5)
		data->key = tryalloc(data->hTable, sizeof(char) * data->keySize);
	if ( val < (5 + data->keySize) )
		return 2;
	if ( val < (5 + data->keySize) )
		data->value = tryalloc(data->hTable, sizeof(char) * data->valueSize);
	if ( val < (9 + data->keySize) )
		return 3;
	if ( val < (9 + data->keySize + data->valueSize))
		return 4;
	return -1;
}

void stringToBinary(char* s) {
    size_t len = strlen(s);
    for(size_t i = 0; i < len; ++i) {
		printf("\n");
        char ch = s[i];
        for(int j = 7; j >= 0; --j){
            if(ch & (1 << j)) { 
				printf("1");
            } else {
                printf("0");
            }
        }
    }
}
int fd_readline_bin(eloop_data* data){
if (data->keySize !=0) printf("%s", data->key);
	int desconectar = 0;
	//int ret, conectado=1, i=0, linea = 0;
	char buffL[READ_SIZE+1];

printf("\n--------\n");
	int rc = read(data->fd, buffL, READ_SIZE  );
	printf("RC = %d\n", rc);
	buffL[rc+1] = '\n';
	stringToBinary(buffL); printf("\n");
	if (rc > 0){
		for (int i = 0; i < rc; i++ ){
			switch (etapaBin(data->cont, data))
			{
			case 0: // Leer el comando
				data->comm = buffL[i];
				printf("[0]");
				//printf("%d\n", data->comm);
				break;
			case 1: // Leer keySize
			printf("[1]");
				data->keySize = data->keySize | (buffL[i] << (8*(4-data->cont)));
				//printf("data->keySize: -%d-\nbuffL[i]: -%d-\n", data->keySize, buffL[i]);
				//printf("| " );convertToBinary(buffL[i]);printf(" |\n" );
				break;
			case 2: // Leer key
				data->key[data->cont - 5] = buffL[i];
				printf("[2]");
				break;
			case 3: // Leer valueSize
				printf("[3]");
				break;
			case 4: // Leer value
				printf("[4]");
				break;
			default: // Procesar request
				printf("5");
				break;
			}
			data->cont++;
		
		}
	}
	
	if (desconectar)
		return 0;
	return rc;
}