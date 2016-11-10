/*
 * st: 127.0.0.1
 *
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */

#include "buffer.h"

read_buffer * 
read_buffer_init(pool_t *p)
{
	read_buffer *rb;
	
	rb =(read_buffer *) palloc(p, sizeof(read_buffer));
	
	rb->ptr = NULL;
	rb->size = 0;

	return rb;
}


read_buffer * 
read_buffer_init_str(pool_t *p, char *b, size_t len)
{
	read_buffer *rb;
	
	rb =(read_buffer *) palloc(p, sizeof(read_buffer));
	
	rb->ptr = b;
	rb->size = len;

	return rb;
}


void 
read_buffer_get_line_split(read_buffer *src, read_buffer *dst, char split)
{
	 char *p, *end;

	if(src == NULL || src->ptr == NULL) return;

	p = src->ptr;
	end =src->ptr + src->size;

	while(p < end){
		if(*p == split){
			dst->ptr = ++p;
			dst->size = end - p;
	 		return;
		}
	 	p++;
	}

	return ;

}


/* turn on in buffer.h*/
void 
read_buffer_get_line(read_buffer *src, read_buffer *dst)
{
	char *p, *end ;
	
	if(src == NULL || dst == NULL) return ;

	p = src->ptr;
	end = src->ptr+src->size;

	if(NULL == p)return;

	while(' ' == *p && p < end)p++;
	dst->ptr =  p;
	for(;   '\r' != *p && '\n' != *p && p < end; p++);

	dst->size = p - dst->ptr; 

	return ; 
}

int 
read_buffer_compare(const read_buffer *s1, const read_buffer *s2)
{
	int len;

	if(s1->size == s2->size) return strncmp(s1->ptr, s2->ptr, s1->size);

	len = s1->size > s2->size?s2->size:s1->size;
	if(len > 1) len--;

	return (s1->ptr[len] - s2->ptr[len]);
	
}

int 
read_buffer_compare_str(const read_buffer *s1, const char *s2)
{
	int len;

	len = strlen(s2);


	if(s1->size == len)  return strncmp(s1->ptr, s2, len);

	len = s1->size >len?len:s1->size;
	if(len > 1) len--;

	return (s1->ptr[len] - s2[len]);
}

void 
read_buffer_to_str_n(read_buffer *b, char *s1, int n)
{
	if(b->ptr == NULL || b->size == 0) {*s1 = 0;return;}


	strncpy(s1, b->ptr, n);
	s1[n] = 0;
}

buffer * 
buffer_init(pool_t *p)
{
	buffer *b;
	
	b = (buffer *)palloc(p, sizeof(buffer));
	
	b->ptr = NULL;
	b->size = 0;
	b->used = 0;

	return b;
}

buffer *
buffer_create_size(pool_t *p, size_t len)
{
	buffer *b = buffer_init(p);
	b->ptr = (char *)palloc(p, len);
	b->size = len;
	b->used = 0;
	
	return b;
}


int  
buffer_get_word_with_split(buffer *src, read_buffer *dst, char split)
{
	char *p, *end;

	dst->size = 0;

	if(src == NULL || src->ptr == NULL)return -1;
	p = src->ptr + src->used;
	end = p + src->size - 1;

	while(' ' == *p && p != NULL) {if(end == p)return -1;p++;}

	dst->ptr = p;

	for(;*p != split; p++) {
		if(end == p) {dst->ptr = NULL;return -1;}
	}

	dst->size = p - dst->ptr;
	src->used += dst->size + 1;

	return 0; 
}


int 
buffer_get_line(buffer *src, read_buffer * dst)
{
	char *p, *end;

	dst->size = 0;

	if(src == NULL || src->ptr == NULL || src->size == 0){dst->size=0;return -1;}
	p = src->ptr + src->used;
	end = src->ptr + src->size - 1;
	while(' ' == *p && p != NULL) {if(end == p)return -1;p++;}

	if(p >= end) {
		dst->size = 0;
		return 0;
	}

	dst->ptr = p;

	for(;*p != '\r' && *p != '\n'; p++)if(end == p){ dst->size = p - dst->ptr + 1; return -1;}

	dst->size = p - dst->ptr;
	src->used = p - src->ptr + 1;
	if(p[1] == '\n') src->used += 1;

	return 0;
}

void  
buffer_clear(buffer *b)
{
	free(b->ptr);
	b->size = 0;
	b->used = 0;
}

list_buffer *
list_buffer_create(pool_t *p)
{
	list_buffer * b;
	
	b = (list_buffer *)palloc(p, sizeof(list_buffer));
	b->b = NULL;
	b->next = NULL;
	
	return b;
}

list_buffer *
list_buffer_add(pool_t *p, list_buffer *lb)
{	
	if(lb->b != NULL) {
		while(lb->next != NULL && lb->b != NULL) lb = lb->next;
		
		if(lb->next != NULL) {
			lb->next = list_buffer_create(p);
			lb = lb->next;
		}
	}

	return lb;
}

list_buffer *
list_buffer_last(list_buffer *lb)
{
	while(lb->next != NULL) lb = lb->next;

	return lb;
}

void
list_buffer_to_lower(list_buffer *lb)
{
	buffer *b ;
	
	for(b = lb->b; lb  != NULL; lb = lb->next) {
		b = lb->b;
		buffer_to_lower(b);
	}
}


void 
buffer_n_to_lower(buffer *src, int n)
{
	if(n > 0) {
		if(src != NULL && src->ptr != NULL) {
			char *p = src->ptr;
			n = n>src->size?src->size:n;
			while((p - src->ptr) < n) {
				if(*p > 'A' && *p < 'Z') {
					*p |= 32;
				}
				p++;
			}
		}
	}
}

void 
buffer_find_str(buffer *src, buffer *dst, char *str) {
	char *p;

	if(src != NULL && src->ptr != NULL && src->size > 0 && str != NULL) {
		p = strstr(src->ptr, str);
		if(p - src->ptr < src->size) {
			dst->ptr = p;
			dst->size = strlen(str);
		}
		else {
			dst->ptr = NULL	;
		}

	}
}


void 
list_buffer_used_to_zero(list_buffer *lb)
{
	while(lb != NULL) {
		lb->b->size = lb->b->used;
		lb->b->used = 0;
		lb = lb->next;
	}
}


int 
buffer_prepare_int(pool_t *p, buffer * b, size_t size)
{
	if((0 == b->size) ||
		(size > b->size))
	{
		if(b->size) free(b->ptr);
		b->size = size;
		b->ptr = (char *)palloc(p, b->size);
	}
	b->used = 0;

	return 0;
}


int 
buffer_to_lower(buffer *b)
{
	char * c ;
	if(0 == b->used) return 0;

	for(c = b->ptr; c-b->ptr <= b->used; c++)
	{
		if (*c >= 'A' && *c <= 'Z') 
		{
			*c |= 32;
		}
	}
	
	return 0;
}

int 
buffer_to_upper(buffer *b)
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


int 
buffer_is_equal(buffer *a, buffer *b) {
	if (a->used != b->used) return 0;

	return (0 == strcmp(a->ptr, b->ptr));
}

int 
buffer_caseless_compare(buffer *a, buffer *b) {
	if (a->used != b->used) return 0;
	if (0 == a->used && 0 == b->used) return 1;

	return (0 == strcasecmp(a->ptr, b->ptr));
}

int 
buffer_is_equal_len(buffer *a, buffer *b, size_t len) {
	if (a->used != b->used) return 0;
	if (0 == a->used && 0 == b->used) return 1;

	return (0 == strncmp(a->ptr, b->ptr, len));
}

int 
buffer_caseless_compare_len(buffer *a, buffer *b) {
	if (a->used != b->used) return 0;
	if (0 == a->used && 0 == b->used) return 1;

	return (0 == strncasecmp(a->ptr, b->ptr, a->used));
}



int 
buffer_path_simplify(buffer *dest, const buffer *src)
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
	dest->used = (out - start);
	return 0;
}

