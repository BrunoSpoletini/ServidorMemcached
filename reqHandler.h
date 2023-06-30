#ifndef __REQHANDLER_H_
#define __REQHANDLER_H_


#include <unistd.h>
#include "structures/hash.h"
#include "structures/Node.h"
#include "structures/stats.h"
#include "structures/dlist.h"
#include "common.h"
#include "socket_handler.h"
#include "utils.h"


void handleConn(eloop_data* data);

void processReq(eloop_data* data, char** req);

int validateReq(eloop_data* data, int words);

int parseLineText(eloop_data* data, char* buff, char** req);

int fd_readline_texto(eloop_data* data);

int fd_readline_bin(eloop_data* data);

#endif