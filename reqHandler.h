#ifndef __REQHANDLER_H_
#define __REQHANDLER_H_


#include <unistd.h>
#include "structures/hash.h"
#include "structures/Node.h"
#include "structures/stats.h"
#include "structures/dlist.h"
#include "common.h"
#include "socket_handler.h"

// Maneja la información que llega por un socket de un cliente
void handleConn(eloop_data* data);

// Procesa y responde la solicitud de un cliente
void processReq(eloop_data* data, char** req);

// Valida si la solicitud recibida es valida
int validateReq(eloop_data* data, int words);

// Parsea una linea que se recibe por un socket de texto
int parseLineText(eloop_data* data, char* buff, char** req);

// Lee la informacion que haya disponible en un socket de texto
int fd_readline_texto(eloop_data* data);

// Lee la informacion que haya disponible en un socket binario
int fd_readline_bin(eloop_data* data);

#endif