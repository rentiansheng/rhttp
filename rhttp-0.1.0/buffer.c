/*
*主要实现功能，对内存的操作。
* 作者：Reage
* blog:www.rhttp.cn
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "buffer.h"
#include <assert.h>



#define BUFFER_PIECE_SIZE 64/*Allocate memory base*/
#define BUFFER_ARRAY_SIZE 16/**/



read_buffer * read_buffer_init()
{
	read_buffer *rb;
	
	rb =(read_buffer *) malloc(sizeof(read_buffer));
	
	rb->ptr = NULL;
	rb->size = 0;

	return rb;
}


read_buffer * read_buffer_init_str(char *b, size_t len)
{
	read_buffer *rb;
	
	rb =(read_buffer *) malloc(sizeof(read_buffer));
	
	rb->ptr = b;
	rb->size = len;

	return rb;
}

/* turn on in buffer.h*/
void  read_buffer_get_string(char *s, read_buffer *b){
		
	char *p = s;
	if(NULL == s)return;
	while( ' ' == *p)p++;
	b->ptr = p ;
	for(; ' ' != *p && '\r' != *p && '\n' != *p; p++);
	b->size = p - b->ptr ; 
	return ; 
	
	
	
}

/* turn on in buffer.h*/
void  read_buffer_get_line(char *s, read_buffer *b){
	char *p = s;	
	if(NULL == s)return;
	while(' ' == *p)p++;
	b->ptr =  p;
	for(;   '\r' != *p && '\n' != *p; p++);
	b->size = p - b->ptr; 
	return ; 
	
	
	
}

void free_read_buffer(read_buffer * b)
{
	free(b);
	return ;
}




buffer * buffer_init()
{
	buffer *b;
	
	b = (buffer *)malloc(sizeof(buffer));
	
	b->ptr = NULL;
	b->size = 0;
	b->used = 0;

	return b;
}






buffer* buffer_init_buffer(buffer *src)
{
	buffer *b = buffer_init();
	buffer_copy_string_buffer(b, src);
	return b;
}

buffer * buffer_init_string(char * str)
{
	buffer *b = buffer_init();

	buffer_copy_string(b, str);
	return b;
}

void  buffer_clear(buffer *b)
{
	free(b->ptr);
	b->size = 0;
	b->used = 0;
}

void buffer_free(buffer *b)
{
	if(NULL != b->ptr)free(b->ptr);
	free(b);
}


int buffer_prepare_int(buffer * b, int size)
{
	if((0 == b->size) ||
		(size > b->size))
	{
		if(b->size) free(b->ptr);
		b->size = size;
		b->size += BUFFER_PIECE_SIZE - (b->size % BUFFER_PIECE_SIZE);
		b->ptr = malloc(b->size);
	}
	b->used = 0;
}

int buffer_prepare_append(buffer *b, size_t size)
{
	if(!b)return -1;
	if(0 == b->size)
	{
		b->size = size;
		b->size += BUFFER_PIECE_SIZE - (b->size % BUFFER_PIECE_SIZE);
		b->ptr = malloc(b->size);
		b->used =0;
	}
	else
	{
		b->size += size;
		b->size += BUFFER_PIECE_SIZE - (b->size % BUFFER_PIECE_SIZE);
		b->ptr = realloc(b->ptr, b->size);
	}
	return 0;
}

int buffer_copy_string_buffer(buffer *b, const buffer *src) {
	if (!src) return -1;

	if (src->used == 0) {
		b->used = 0;
		return 0;
	}
	return buffer_copy_string_len(b, src->ptr, src->used - 1);
}

int buffer_copy_string(buffer *b, const char *s)
{
	size_t len;
	if(!s || !b)return -1;
	
	len = strlen(s) + 1;
	
	buffer_prepare_int(b, len);
	memcpy(b->ptr, s, len);
	b->used = len;
	return 0;
}

int buffer_copy_string_len(buffer *b, const char *s, size_t s_len)
{
	if(!s || !b)return -1;
	
	buffer_prepare_int(b,s_len + 1);
	
	memcpy(b->ptr, s, s_len);
	b->used = s_len + 1;
	b->ptr[s_len] = '\0';
	return 0;
	
}

int buffer_append_string_buffer(buffer *b, const buffer *src)
{
	if(!b || !src) return -1;
	if(0 == src->used)
	{
		b->used = 0;
		return 0;
	}
	return buffer_copy_string_len(b, src->ptr, src->used);
	
}

int buffer_append_string(buffer *b, const char *s)
{
	size_t s_len;
	
	if(!b || !s) return -1;

	s_len = strlen(s);
	buffer_prepare_append(b, s_len + 1);
	if (b->used == 0)
		b->used++;

	memcpy(b->ptr + b->used - 1, s, s_len + 1);
	b->used += s_len;

	return 0;
}

int buffer_append_memory(buffer *b, const char *s, size_t s_len)
{
	if(!b || !s) return -1;
	if(0 == s_len) return 0;

	buffer_prepare_append(b, s_len);

	memcpy(b->ptr + b->used, s, s_len);
	b->used += s_len;
	return 0;	
}

int buffer_copy_memory(buffer *b, const char *s, size_t s_len)
{
	if (!s || !b) return -1;
	if (s_len == 0) return 0;

	buffer_prepare_append(b, s_len);
	memcpy(b->ptr + b->used, s, s_len);
	b->used += s_len;

	return 0;
}

int buffer_to_lower(buffer *b)
{
	char * c ;
	if(0 == b->used) return 0;

	for(c = b->ptr; *c; c++)
	{
		if (*c >= 'A' && *c <= 'Z') 
		{
			*c |= 32;
		}
	}
	
	return 0;
}

int buffer_to_upper(buffer *b)
{
	char * c ;

	if(0 == b->used) return 0;

	for(c = b->ptr; *c; c++)
	{
		if (*c >= 'A' && *c <= 'Z') 
		{
			*c &= 223;  // *c &= ~32;
		}
	}
	
	return 0;
}


int buffer_is_equal(buffer *a, buffer *b) {
	if (a->used != b->used) return 0;

	return (0 == strcmp(a->ptr, b->ptr));
}

int buffer_caseless_compare(buffer *a, buffer *b) {
	if (a->used != b->used) return 0;
	if (0 == a->used && 0 == b->used) return 1;

	return (0 == strcasecmp(a->ptr, b->ptr));
}

int buffer_is_equal_len(buffer *a, buffer *b, size_t len) {
	if (a->used != b->used) return 0;
	if (0 == a->used && 0 == b->used) return 1;

	return (0 == strncmp(a->ptr, b->ptr, len));
}

int buffer_caseless_compare_len(buffer *a, buffer *b) {
	if (a->used != b->used) return 0;
	if (0 == a->used && 0 == b->used) return 1;

	return (0 == strncasecmp(a->ptr, b->ptr, a->used));
}



buffer_array* buffer_array_init(void) {
	buffer_array *b;

	b = malloc(sizeof(*b));

	assert(b);
	b->ptr = NULL;
	b->size = 0;
	b->used = 0;

	return b;
}

/**
 * free the buffer_array
 *
 */

void buffer_array_free(buffer_array *b) {
	size_t i;
	if (!b) return;

	for (i = 0; i < b->size; i++) {
		if (b->ptr[i]) buffer_free(b->ptr[i]);
	}
	free(b->ptr);
	free(b);
}


#define BUFFER_PIECE_SIZE 64

int buffer_prepare_copy(buffer *b, size_t size) {
	if (!b) return -1;

	if ((0 == b->size) ||
	    (size > b->size)) {
		if (b->size) free(b->ptr);

		b->size = size;

		/* always allocate a multiply of BUFFER_PIECE_SIZE */
		b->size += BUFFER_PIECE_SIZE - (b->size % BUFFER_PIECE_SIZE);

		b->ptr = malloc(b->size);
		assert(b->ptr);
	}
	b->used = 0;
	return 0;
}


/*

buffer * buffer_array_append(buffer_array *b)
{
	size_t i;
	if(0 == b->size)
	{
		b->size = BUFFER_ARRAY_SIZE;
		b->ptr = malloc(sizeof(*b->ptr) * BUFFER_ARRAY_SIZE);
		assert(b);
		for(i = 0; i < BUFFER_ARRAY_SIZE; i++)
		{
			b->ptr[i] = NULL;
		}
		b->used = 0;
	}
	else if (b->used  == b->size)
	{
		b->size += BUFFER_ARRAY_SIZE;
		b->ptr = realloc(b->ptr,sizeof(char ) * b->size);
		assert(b);
		for(i = used - 1; i < b->size; i++)
		{
			b->ptr[i] = NULL;
		}
	}
	if(NULL == b->ptr[b->used])
	{
		b->ptr[b->used] = buffer_init();
	}
	return b->ptr[b->used++];
}
*/

int buffer_path_simplify(buffer *dest, const buffer *src)
{
	int count;
	char c;
	/*
		start保存结果路径的开始位置。
		slash本次配皮的开始位置，
		walk正在匹配的字符
		out将要写入的结果的位置
	*/
	char *start, *slash, *walk, *out;
	
	char pre; //当前匹配的前一个字符

	if (src == NULL ||  dest == NULL)
		return -1;


	walk  = src->ptr;
	start = dest->ptr;
	out   = dest->ptr;
	slash = dest->ptr;



	while (*walk == ' ') {
		walk++;
	}

	pre = *(walk++);
	c    = *(walk++);
	if (pre != '/') {
		*(out++) = '/';
	}
	*(out++) = pre;

	if (pre == '\0') {
		*out = 0;
		return 0;
	}

	while (1) {
		// '/'表示本次匹配的结束，'\0'表示整个过程的结束
		if (c == '/' || c == '\0') {
			//计算本次共匹配到的字符 即使'/'与'/'的距离
			count = out - slash;
			//说明本次匹配为../需要删除前面的目录
			if (count == 3 && pre == '.') {
					
				//首先回到本次匹配的开始，即使../的一个点的位置，
				out = slash;
				//回溯到前面的一个目录，只要前面不是根目录
				if (out > start) {
					out--;
					while (out > start && *out != '/') {
						out--;
					}
				}

				if (c == '\0')
					out++;
			} else if (count == 1 || pre == '.') {
				//本次匹配为“./”只要删除掉就可。即使回到本次开始匹配的位置
				out = slash;
				if (c == '\0')
					out++;
			}

			slash = out;
		}

		if (c == '\0')
			break;
		//先将内容写入，本保存上一个的内容
		pre = c;
		c    = *walk;
		*out = pre;

		out++;
		walk++;
	}

	*out = '\0';
	dest->used = (out - start) + 1;
	return 0;
}



