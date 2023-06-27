#include "utils.h"

char *itos(int i){
	char *str = malloc(sizeof(char) * 10);
	sprintf(str, "%d", i);
	return str;
}

bool printable(char *s, int len){
    bool rta = true;
    for(int i = 0;i < len; i++){
        if( s[i] < 32 || s[i] >= 127 )
            rta = false;
    }
    return rta;
}
