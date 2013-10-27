#include <string.h>
#include "memory.h"
#include "base.h"

struct KEY * key_init( char *name, char *value)
{
	struct KEY *  k;
	k = (struct KEY * ) r_malloc(sizeof(struct KEY));
	k->value = (char *) r_malloc(sizeof(char)*strlen(value));
	strcpy(k->value,value);
	k->name = (char *) r_malloc(sizeof(char)*strlen(name));
	strcpy(k->name,name);
	return k;
}

char * key_indexof_key(struct KEY *key, char * name)
{
	char *p = NULL;
	struct KEY *k = key;
	while(k){
		if(0 == strcmp(k->name, name)){
			p = k->value;
			break;
		}
		k = k->next;
	}
	return p;
}
