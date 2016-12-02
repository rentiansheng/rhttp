#include "http_request_header.h"



static int read_http_header(buffer *header, pool_t *p, int fd)
{
	char c;

	while(read(fd, &c, 1)) {
		buffer_append_char(header,c,p);
		if(c == '\n' && header->used >= 2) {
			char *ptr =  header->ptr + header->used - 2;
			if(strncasecmp(ptr, "\n\n", 2) == 0) {
				return 0;
			}
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


	p =(pool_t *) con->p;

	if(con->in->header == NULL) {
		con->in->header = buffer_init(p);
	}

	
	return read_http_header(con->in->header , p, con->fd);
}

int read_cgi(epoll_cgi_t *cgi)
{
	pool_t *p; 
	buffer *header;
	
	p = cgi->con->p;
	if(cgi->cgi_data == NULL){
		cgi->cgi_data = buffer_init(p);
	}

	header = cgi->cgi_data;

	return read_http_header(header, p, cgi->fd);
	
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
	if(strncasecmp(start, "get ", 4) == 0) {
		in->http_method = _GET;
		start += 3;
	}
	else if(strncasecmp(start, "post ", 5) == 0) {
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
	

}

void parse_header(http_connect_t * con)
{
 	struct request *in;
	buffer *header;
	buffer *b;
	string *line;
	string *k,*v;
	pool_t *p;
	
	char *start, *end;
	


	p = (pool_t *)con->p;
	line = (string *)string_init(p);
	k = (string *)string_init(p);
	v = (string *)string_init(p);
	in = con->in;
	header = in->header;

	start = in->header->ptr;
	end = in->header->ptr + in->header->used;

	string_get_line(start, end, line);

	//buffer_get_line(header, dst);

	parse_http_uri(p, in, line);

	start = line->ptr + line->len;
	while(*start == '\r' || *start == '\n' ) {
		start ++;
	}
	while(start < end) {
		string_get_line(start, end, line);
		string_split_kv(line, k, v, ':');
		string_skip_char(k, ' ');
		string_skip_char(v, ' ');
		if(k->len == 15 && strncasecmp(k->ptr, "accept-encoding", 15) == 0) {

			if(v->len == 0) {
				in->accept_encoding = _NOCOMPRESS;
			}
			else if(v->len == 4 && strncasecmp(v->ptr, "gzip", 4) == 0) {
				in->accept_encoding = _GZIP;
			}
			else if(v->len == 7 && strncasecmp(v->ptr, "deflate", 7) == 0) {
				in->accept_encoding = _DEFLATE;
			}
		}
		else if(k->len == 4 && strncasecmp(k->ptr, "host", 4) == 0) {		
			in->host = v;
		}
		else if(k->len == 13 && strncasecmp(k->ptr, "authorization", 13) == 0) {
			in->authorization = v;
			decoded_usr_pwd(con);
		}
		else if(k->len == 14 && strncasecmp(k->ptr, "content-length", 14) == 0) {
			in->content_length = v;
		}

		start = line->ptr + line->len;
		while(*start == '\r' || *start == '\n' ) {
			start ++;
		}

	}


	//test_print_header(in);
	return ;
}
