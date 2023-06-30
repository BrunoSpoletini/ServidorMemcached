#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

const char *code_str(enum code e)
{
	switch (e) {
	case PUT:	return "PUT";
	case GET:	return "GET";
	case DEL:	return "DEL";

	case STATS:	return "STATS";

	case OK:	return "OK";
	case EINVALID:	return "EINVALID";
	case ENOTFOUND:	return "ENOTFOUND";
	case EBINARY:	return "EBINARY";
	case EBIG:	return "EBIG";
	case EUNK:	return "EUNK";

	default:
		assert(0);
		return "";
	}
}

void quit(char *s){
	perror(s);
	abort();
}

int str_to_comm(char *str){
	if (strcmp(str, "PUT") == 0)
		return PUT;
	if (strcmp(str, "GET") == 0)
		return GET;
	if (strcmp(str, "DEL") == 0)
		return DEL;
	if (strcmp(str, "STATS") == 0)
		return STATS;
	return -1;
}