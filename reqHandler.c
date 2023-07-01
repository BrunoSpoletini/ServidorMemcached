#include "reqHandler.h"

/* Common */

void handleConn(eloop_data* data)
{	
	int rc = data->isText? fd_readline_texto(data) : fd_readline_bin(data);

	 if ( rc == -1 )
	 	quit("Fallo al leer");

	if ( rc == 0 ){
		printf("cliente desconectado\n");
		desconectarCliente(data);
	}

	 if ( rc > 0 ){
		agregarClienteEpoll(data->fd, data->epfd, 0, (void*)data, data->hTable);
	} 
}

void writeSock(eloop_data* data, int resp, bool endline){
	if ( data->isText ){
		char val[15];
		strcpy(val, code_str(resp));
		write(data->fd, val, strlen(val));
		if(endline){
			write(data->fd, "\n", 1);
		}else{
			write(data->fd, " ", 1);
		}
	} else {
		int val = resp;
		write(data->fd, &val, 1);
	}
}

void processReq(eloop_data* data, char** req){
	
	printf("\n%d-%s-%s-\n", data->comm, req[0], req[1]);
	
	if ( data->comm == PUT && req[2] == NULL)
	{	
		Node* nodo;
		if (data->isText){
			nodo = create_node_from_KV(data->hTable, req[0], strlen(req[0]), req[1], strlen(req[1]));//Podemos guardar los strlen en keysize y valuesize asi es mas homogeneo - DEBUG
		} else {
			nodo = create_node_from_KV(data->hTable, req[0], data->keySize, req[1], data->valueSize);
		}

		_PUT(data->hTable, nodo);
		writeSock(data, OK, true);
	}
	else if (( data->comm == DEL ) && req[1] == NULL)
	{
		Node* nodo = create_node_from_K(data->hTable,req[0], strlen(req[0])); 

		int res = _DEL(data->hTable, nodo);
		writeSock(data, res, true);
	}
	else if (( data->comm == GET ) && req[1] == NULL)
	{
		Node* nodo = create_node_from_K(data->hTable,req[0], strlen(req[0])); 
		char* res = NULL;
		int size;
		bool printable;
		int ret = _GET(data->hTable, nodo, &res, &size, &printable);

		if(ret == OK){

			if(size > 2045){ /// por el OK 
				writeSock(data, EBIG, true);
				goto done;
			}
			
			if(!printable){
				writeSock(data, EBINARY, true);
				goto done;
			}

			writeSock(data, ret, false);
			write(data->fd, res, size);
			write(data->fd, "\n", 1);

		}else{
			writeSock(data, ret, true);
		}
		done:

		/*
		if ( ret == OK ){
			if ( data->isText ){ //DEBUG - aca hay que agregar si es imprimible
				write(data->fd, res, size);
				write(data->fd, "\n", 1);
			} else {
				// TO DO - Creo que hay que cambiar los size en todos lados por int32_t, pq 
				// los de texto entran, pero los de binario no.
				int val = htonl(size);
				write( data->fd, &val, 4 );
				write( data->fd, res, val);
			}
		}
		*/


	} else if (( data->comm == STATS ) && req[0] == NULL){
		printf("Stat\n");
		//do stuff
	} 
}

int validateReq(eloop_data* data, int words){
	if (((data->comm == PUT ) && words == 3) ||
		((data->comm == GET) && words == 2) ||
		((data->comm == DEL) && words == 2) ||
		((data->comm == STATS) && words == 1)) {
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
		return -1;
	for (int i = 0; token != NULL && words <= 3; i++)
	{
		words++;
		if ( i == 0 ){
			data->comm = str_to_comm(token);
		} else {
			req[i-1] = malloc(sizeof(char)*(strlen(token) + 1));
			if (req[i-1] == NULL)
				quit("Fallo malloc");
			strcpy(req[i-1], token);
			req[i-1][strlen(token) + 1] = '\0';
		}
		req[i] = NULL;
		token = strtok(NULL, " \n");
	}
	printf("Words: [%d]\n", words);
	if (words > 0 && words <= 3 && validateReq(data, words))
		return 1;
	return 0;
}

int fd_readline_texto(eloop_data* data)
{
	int conectado=1, i=0, linea = 0;
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
					if ( conectado )
					{
						char **req = malloc(sizeof(char*) * 2);
						switch ( parseLineText(data, buffer + linea, req) )
						{
						case 0:
							write(data->fd, "Comando invalido\n", 17);
							break;
						case 1:
							processReq(data, req);
							break;
						default:
							conectado = 0;
							break;
						}
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

void parseBin(eloop_data* data){
	char* req[2];
	switch (data->comm)
	{
	case 11: //PUT
		//if ( data->cont == data->keySize + data->valueSize + 9 ){
			req[0] = data->key;
			req[1] = data->value;
			req[2] = NULL;
			processReq(data, req);
		//}
		break;
	case 12: //DEL
		//if ( data->cont == data->keySize + 5 ){
			req[0] = data->key;
			req[1] = NULL;
			processReq(data, req);
		//}
		break;
	case 13: //GET
		//if ( data->cont == data->keySize + 5 ){
			req[0] = data->key;
			req[1] = NULL;
			processReq(data, req);
		//}
		break;
	case 21: //STATS
		//if ( data->cont == 1 ){
			req[0] = NULL;
			processReq(data, req);
		//}
		break;
	default:
		printf("ParseBin...raro\n"); //DEBUG
		processReq(data,req);
		break;
	}
	data->cont = 0;
	data->keySize = 0;
	data->valueSize = 0;
}

/* Handler binario */
int etapaBin( eloop_data* data ){
	int val = data->cont;

	if ( val < 1) // Comando
		return 0;

	//Procesar STATS
	if ( (val == 1) && (data->comm == STATS) ){ 
		parseBin(data);
		return 0;
	}

	if ( val < 5 ) // keySize
		return 1;

	if ( val == 5 ){ // malloc a key
		char* keyB = tryalloc(data->hTable, sizeof(char) * data->keySize);
		data->key = keyB;
	}
	if ( val < (5 + data->keySize) ) // key
		return 2;

	// Procesar GET o DEL
	if ( (val == (5 + data->keySize) ) && ( (data->comm == GET) || (data->comm == DEL) ) ){ 
		parseBin(data);
		return 0;
	}

	if ( val < (9 + data->keySize) ) // valueSize
		return 3;

	if ( val == (9 + data->keySize) ){ // malloc a value
		char* value = tryalloc(data->hTable, sizeof(char) * data->valueSize);
		data->value = value;
	}
	if ( val < (9 + data->keySize + data->valueSize)) // value
		return 4;

	// Procesar PUT
	if ( (val == (9 + data->keySize + data->valueSize) ) && (data->comm == PUT ) ){ 
		parseBin(data);
		return 0;
	}
	return -1;
}

int fd_readline_bin(eloop_data* data){
	char buffL[READ_SIZE+1];
	int rc = 0;
	printf("esperando para leer\n");
	rc = read(data->fd, buffL, READ_SIZE);
	printf("saliendo de leer, RC = %d\n", rc);
	if (rc > 0){
		for (int i = 0; i < rc; i++ ){
			switch (etapaBin( data ))
			{
			case 0: // Leer el comando
				data->comm = buffL[i];
				printf("[0]");
				
				break;
			case 1: // Leer keySize 
				printf("[1]");
				data->keySize = data->keySize | (buffL[i] << (8*(4-data->cont)));
				break;
			case 2: // Leer key
				printf("[2]");
				data->key[data->cont - 5] = buffL[i];
				break;
			case 3: // Leer valueSize
				printf("[3]");
				data->valueSize = data->valueSize | (buffL[i] << (8*(8 + data->keySize - data->cont)));
				break;
			case 4: // Leer value
				printf("[4]");
				data->value[data->cont - data->keySize - 9] = buffL[i];
				break;
			default:
				//parseBin(data);
				quit("Error en el parser binario\n");
				break;
			}
			data->cont++;
		}
		parseBin(data);
	}
	
	return rc; //En teoria si rc == 0, el handle_conection desconecta al cliente
}