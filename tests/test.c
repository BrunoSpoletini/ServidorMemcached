
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/ip.h>


char * input(int *lenp)
{
	printf("1\n");
	char *ret = malloc(sizeof(char)*1024);
	if (ret == NULL){printf("NULL\n");}
	int off = 0, sz = 1024;
	int rc;
	printf("2\n");
	while ((rc = read(0, ret + off, sz - off)) > 0) {
		off += rc;
		if (off == sz) {
			sz *= 2;
			ret = realloc(ret, sz);
		}
        printf("RET: -%s-\n", ret);

        if( ret[sz] == '\0'){
            break;
        }

	}

        printf("RC: %d\n", rc);


	if (rc < 0){
		printf("input.read?\n");
        return NULL;
    }
	printf("4\n");

	/* OK, EOF */
	*lenp = off;
	return ret;
}

int main(){
    char* ret;
    int len;
    ret = input(&len);
    printf("--%s, %d--",ret, len);
    return 0;
}