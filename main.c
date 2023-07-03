#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/resource.h>
#include "common.h"
#include "socket_handler.h"

int tsocket = DEFAULT_TEXT_SOCKET;
int bsocket = DEFAULT_BIN_SOCKET;
unsigned long long mlimit = DEFAULT_MEM_LIMIT;
int nthreads = DEFAULT_MAX_THREADS;

/*
Este archivo main deberia ser el primero en llamar, con permisos de sudo.
Seteamos el limite de memoria, 
Luego, levantamos los sockets, bajamos privilegios, y pasamos al server los sockets.
*/

int lsock_tcp(int port)
{
	struct sockaddr_in sa;
	int lsock;
	int rc;
	int yes = 1;

	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock < 0)
		quit("socket");

	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	rc = listen(lsock, MAX_CLIENTS_QUEUE);

	return lsock;
}

bool std_down_privileges(){

	if (getuid() != 0)
		return true;

	char *s =getenv("SUDO_UID"); /// usuario que llamo con sudo.

	if( setuid( atoi(s) ) == -1 )
		return false;

	if( getuid() != 0)
		return true;
	
    return false;
}

void parseargs(int argc, char **argv){
	for (int i = 1; i < argc - 1; i++){
		if (strcmp(argv[i], "-t") == 0){
            tsocket = atoi(argv[i + 1]);
            if(tsocket < 0 || tsocket > 65535)
                quit("Puerto de socket de texto no valido\n");
        }
		if (strcmp(argv[i], "-b") == 0){
			bsocket = atoi(argv[i + 1]);
            if(bsocket < 0 || bsocket > 65535)
                quit("Puerto de socket binario no valido\n");
        }
		if (strcmp(argv[i], "-m") == 0){
			mlimit = atoi(argv[i + 1]);
            if(mlimit < 0)
                quit("Limite de memoria invalido\n");
        }
		if (strcmp(argv[i], "-nt") == 0){
			nthreads = atoi(argv[i + 1]);
            if(nthreads < 0)
                quit("cantidad de threads invalida\n");
        }
	}
}

int main(int argc, char **argv){

	parseargs(argc, argv);

	uid_t uid = getuid(); /// les pasamos user y grupo de usuario no root.
	if (uid != 0)
		quit("Se necesitan permisos de root.");

	int text_socket = lsock_tcp(tsocket);
	int bin_socket = lsock_tcp(bsocket);

	struct rlimit *rlimits = malloc(sizeof(struct rlimit));
	rlimits->rlim_cur = mlimit;		
	rlimits->rlim_max = rlimits->rlim_cur;     /// hard limit = soft limit.

	if (setrlimit(RLIMIT_AS, rlimits) == -1) /// RLIMIT_AS -> Adress Space If the limit is exceeded, malloc() and mmap() functions will fail with an errno of ENOMEM
		quit("No se pudo limitar la memoria");

	free(rlimits);

	if (!std_down_privileges())
        quit("No se pueden bajar los privilegios.\n");  

    if (access("./server", F_OK) == 0){
        execl("./server" , "./server" , itos(text_socket) , itos(bin_socket) , itos(nthreads) ,  NULL); 
	}else{
		quit("El server no fue buildeado");
	}
	return 0;
}
