#include <stdio.h>
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

// unsigned long hash(const char* key, int len) { /// esta aca de momento esta funcion.
//     unsigned long hashValue = 0;
//     int i = 0;
//     for(int i = 0;i < len; i++){
//         hashValue = key[i] + (hashValue << 5) - hashValue; /// estaria bueno saber si con esto nos evitamos los overflows.
//     }
//     return hashValue;
// }
