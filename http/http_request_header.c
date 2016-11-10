#include "http_request_header.h"


static int read_fd(list_buffer *header, pool_t *p, int fd)
{
	list_buffer *lb, *tmplb;
	buffer *b;
	int palloc_size = 1024;
	int count;
	int size;

	lb = list_buffer_last(header);
	if(lb->b ==	NULL) {
		lb->b = buffer_create_size(p, palloc_size);
		memset(lb->b->ptr, 0, 1024);
	}

	
	b = lb->b;
	while(( size = (size_t)read(fd, b->ptr + b->used, (int)(b->size - b->used))) >= 0) {
		b->used += size;
		if(size == 0){
			return 0;
		}
		if(b->size == b->used ) {
			tmplb  = list_buffer_add(p, header);
			tmplb->b = buffer_create_size(p, palloc_size);
			adjust_header(lb, tmplb);
			lb= tmplb;
			b = lb->b;
		}
	}

	return 1;
}

static int read_http_header(list_buffer *header, pool_t *p, int fd)
{
	list_buffer *lb, *tmplb;
	buffer *b;
	int palloc_size = 1024;
	int count;
	int size;

	lb = list_buffer_last(header);
	if(lb->b ==	NULL) {
		lb->b = buffer_create_size(p, palloc_size);
		memset(lb->b->ptr, 0, 1024);
	}

	
	b = lb->b;
	while(( size = (size_t)read(fd, b->ptr + b->used, (int)(b->size - b->used))) >= 0) {
		b->used += size;
		if(size == 0){
			return 0;
		}
		else if((count = find_header_end(b)) >= 0) {
			//list_buffer_to_lower(header);
			return 0;
		}
		if(b->size == b->used ) {
			tmplb  = list_buffer_add(p, header);
			tmplb->b = buffer_create_size(p, palloc_size);
			adjust_header(lb, tmplb);
			lb= tmplb;
			b = lb->b;
		}
	}

	return 1;
}

/**
 *0 读取结束，其他没有结束
 *
 */
int read_header(http_connect_t *con)
{
	pool_t *p;
	list_buffer *header;


	p =(pool_t *) con->p;

	if(con->in->header == NULL)con->in->header = list_buffer_create(p);
	header = con->in->header;
	
	return read_http_header(header, p, con->fd);
}

int read_cgi(epoll_cgi_t *cgi)
{
	pool_t *p; 
	list_buffer *header;
	
	p = cgi->con->p;
	if(cgi->cgi_data == NULL)cgi->cgi_data = list_buffer_create(p);
	header = cgi->cgi_data;

	return read_fd(header, p, cgi->fd);
	
}

static int find_header_end(buffer *b)
{
	char *ptr = b->ptr;
	char *end = b->ptr + b->used ;

	int count = 0;
	
	while(ptr[0] != '\r' && ptr[1] != '\n' 
		&& ptr[2] != '\r' && ptr[2] != '\n') {
		ptr ++;
		count++;
	 	if( (ptr + 4) >=  end){return -1;}
	}

	 return ptr - b->ptr + 4;
}

static void adjust_header(list_buffer *pre, list_buffer *last)
{
	int count = 0;
	char *ptr;

	ptr = pre->b->ptr + pre->b->used - 1;
	if(*ptr == '\n' && ptr[-1] == '\r') return;
	
	ptr--;

	while(*ptr !='\n' && ptr[-1] != '\r') {
		if(ptr == pre->b->ptr)return ; 
		ptr--;
		count ++;
	}

	ptr++;
	pre->b->size = ptr- pre->b->ptr;
	strncpy(last->b->ptr, ptr, count);
	last->b->used = count;

	return ;
}

static char * skip_space(char *start, char *end)
{
	if(start == NULL) return NULL;
	while( *start == ' ') {
		if(start >= end) return NULL;
		start ++;
	}

	return start;
}

static char *find_end(char *start, char *end) 
{
	while(*start != ' ') {
		if(start >= end) return end;
		start ++;
	}

	return start;
}


static void parse_http_uri(struct pool_t *p, request *in, string *rb)
{
	char *ptr;
	char *start;
	char *end;
	int count;

	start = rb->ptr;
	end = rb->ptr+rb->len;

	start = skip_space(start, end);
	if(strncasecmp(start,"get", 3) == 0) {
		in->http_method = _GET;
		start += 3;
	}
	else if(strncasecmp(start, "post", 4) == 0) {
		in->http_method = _POST;
		start +=4;
	}
	else {
		in->http_method = _NONE;
		return ;
	}

	start = (char *)skip_space(start+1, end);
	in->uri = string_init(p);
	in->uri->ptr = start;
	ptr = find_end(start, end);
	in->uri->len = ptr - start;
	start = ptr + 1;

	in->args = (string *)string_init(p);

	ptr = strchr(in->uri->ptr, '?');
	if(ptr != NULL && (ptr - in->uri->ptr)<in->uri->len) {
		count  = ptr - in->uri->ptr;
		in->args->ptr = ptr + 1;
		in->args->len = in->uri->len - count - 1;
		in->uri->len = count;
	}

	if(start == end) return ;
	in->http_version = string_init(p);
	in->http_version->ptr = start ;
	in->http_version->len = end - start;
	
	return;
}


static void test_print_header(request *in)
{
	printf("uri=%s  len =%d|", in->uri->ptr, in->uri->len);
	printf("host=%s  len =%d|", in->host->ptr, in->host->len);
	printf("http_version=%s  len =%d|", in->http_version->ptr, in->http_version->len);

	printf("http_method:");
	if(in->http_method == _GET) {
		printf("get\n");
	}
	else if(in->http_method == _POST) {
		printf("post\n");
	}
	else {
		printf("none\n");
	}

	printf("accept_encoding:");
	if(in->accept_encoding == _DEFLATE) {
		printf("deflate\n");
	}
	else if(in->accept_encoding == _GZIP) {
		printf("gzip\n");
	}
	else {
		printf("none\n");
	}
	
	printf("%s", in->header->b->ptr);

}

void parse_header(http_connect_t * con)
{
 	struct request *in;
	struct list_buffer *header;
	buffer *b;
	string *dst;
	pool_t *p;
	


	p = (pool_t *)con->p;
	dst = (string *)palloc(p, sizeof(string));
	in = con->in;
	header = in->header;
	
	list_buffer_used_to_zero(header);
	b = header->b;
	buffer_get_line(b, dst);
	parse_http_uri(p, in, dst);
	
	while(!buffer_get_word_with_split(b, dst, ':')) {
		if(dst->ptr == NULL && header->next == NULL) return ;
		if(dst->ptr == NULL) { header = header->next;b = header->b;continue;}
		if(strncasecmp("accept-encoding", dst->ptr, dst->len) == 0) {
			buffer_get_line(b, dst);
			if(dst->len == 0) {
				in->accept_encoding = _NOCOMPRESS;
			}
			else if(strstr(dst->ptr, "gzip") != NULL) {
				in->accept_encoding = _GZIP;
			}
			else if(strstr(dst->ptr, "deflate") != NULL) {
				in->accept_encoding = _DEFLATE;
			}
		}
		else if(strncasecmp("host", dst->ptr, dst->len) == 0) {
			buffer_get_line(b, dst);
			in->host = (string *)palloc(p, sizeof(string));
			in->host->ptr = dst->ptr;
			in->host->len = dst->len;
		}
		else if(strncasecmp("authorization", dst->ptr, dst->len) == 0) {
			buffer_get_line(b, dst);
			in->authorization = (string *)palloc(p, sizeof(string));
			in->authorization->ptr = dst->ptr;
			in->authorization->len = dst->len;
			decoded_usr_pwd(con);
		}
		else if(strncasecmp("content-length",dst->ptr, dst->len) == 0) {
			buffer_get_line(b, dst);
			in->content_length = (string *)pcalloc(p, sizeof(string));
			in->content_length->ptr = dst->ptr;
			in->content_length->len = dst->len;

		}
		else {
			buffer_get_line(b, dst);
			
		}
	}

	//test_print_header(in);
	return ;
}
