#include <stdlib.h>

void *r_malloc(size_t size)
{
	void * result =  malloc(size);
	if(NULL == result)
	{
		printf("\nFailed to allocate memory!\n");
		exit(0);
	}
	return result;
}

void *r_realloc(void *ptr,size_t size)
{
	void * result =  realloc(ptr,size);
	if(NULL == result)
	{
		printf("\nFailed to allocate memory!\n");
		exit(0);
	}
	return result;
}
