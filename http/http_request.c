#include "http_request.h"



static int find_header_end(buffer *b);

static void adjust_header(list_buffer *pre, list_buffer *last);

response * 
response_init(pool_t *p)
{
	response *out;
    
 	out = palloc(p, sizeof(response));
 	out->status_code = HTTP_UNDEFINED;
	out->content_length = 0;
	out->server  = NULL;
	out->date = NULL;
	out->www_authenticate = NULL;
	out->content_type = NULL;
	out->physical_path = NULL;
	out->content_encoding = _NOCOMPRESS;

	return out;
}

request * 
request_init(pool_t *p)
{
	request *in;

	in = pcalloc(p, sizeof(request));

	return in;
}
 


static int
start_web_server(struct http_conf *g)
{
	struct web_conf *web;
	int count ;
	int epfd;
	int fd = 0;
	epoll_extra_data_t *data;

	count = g->web_count;
	web = g->web;

	data = (epoll_extra_data_t *) malloc(sizeof(epoll_extra_data_t));

	data->type = SERVERFD;
	if(g->port <=0) g->port = 80;
	epfd = epoll_init(MAX_CONNECT);
	//while(count--){
	fd = socket_listen("127.0.0.1", g->port);
	//	web->fd = fd;
	epoll_add_fd(epfd, fd,EPOLL_R, data);
	//	web = web->next;
	//}
	
	g->fd = fd;
	g->epfd = epfd;

	return epfd;
}

static int
read_fd(list_buffer *header, pool_t *p, int fd)
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
			printf("%s\n");
		if(size == 0){
			printf("\n-------------------\n%s\n");
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

static int
read_http_header(list_buffer *header, pool_t *p, int fd)
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
static int
read_header(http_connect_t *con)
{
	pool_t *p;
	list_buffer *header;


	p =(pool_t *) con->p;

	if(con->in->header == NULL)con->in->header = list_buffer_create(p);
	header = con->in->header;
	
	return read_http_header(header, p, con->fd);
}

static int
read_cgi(epoll_cgi_t *cgi)
{
	pool_t *p; 
	list_buffer *header;
	
	p = cgi->con->p;
	if(cgi->cgi_data == NULL)cgi->cgi_data = list_buffer_create(p);
	header = cgi->cgi_data;

	return read_fd(header, p, cgi->fd);
	
}

static int 
find_header_end(buffer *b)
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

static void 
adjust_header(list_buffer *pre, list_buffer *last)
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

static char *
skip_space(char *start, char *end){
	if(start == NULL) return NULL;
	while( *start == ' ') {
		if(start >= end) return NULL;
		start ++;
	}

	return start;
}

static char *
find_end(char *start, char *end) {
	while(*start != ' ') {
		if(start >= end) return end;
		start ++;
	}

	return start;
}


static void
parse_http_uri(struct pool_t *p, request *in, read_buffer *rb)
{
	char *ptr;
	char *start;
	char *end;
	int count;

	start = rb->ptr;
	end = rb->ptr+rb->size;

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
	in->uri = read_buffer_init(p);
	in->uri->ptr = start;
	ptr = find_end(start, end);
	in->uri->size = ptr - start;
	start = ptr + 1;

	in->args = (read_buffer *)read_buffer_init(p);

	ptr = strchr(in->uri->ptr, '?');
	if(ptr != NULL && (ptr - in->uri->ptr)<in->uri->size) {
		count  = ptr - in->uri->ptr;
		in->args->ptr = ptr + 1;
		in->args->size = in->uri->size - count - 1;
		in->uri->size = count;
	}

	if(start == end) return ;
	in->http_version = read_buffer_init(p);
	in->http_version->ptr = start ;
	in->http_version->size = end - start;
	
	return;
}


static void test_print_header(request *in)
{
	printf("uri=%s  len =%d|", in->uri->ptr, in->uri->size);
	printf("host=%s  len =%d|", in->host->ptr, in->host->size);
	printf("http_version=%s  len =%d|", in->http_version->ptr, in->http_version->size);

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

static void 
parse_header(http_connect_t * con)
{
 	struct request *in;
	struct list_buffer *header;
	buffer *b;
	read_buffer *dst;
	pool_t *p;
	


	p = (pool_t *)con->p;
	dst = (read_buffer *)palloc(p, sizeof(read_buffer));
	in = con->in;
	header = in->header;
	
	list_buffer_used_to_zero(header);
	b = header->b;
	buffer_get_line(b, dst);
	parse_http_uri(p, in, dst);
	
	while(!buffer_get_word_with_split(b, dst, ':')) {
		if(dst->ptr == NULL && header->next == NULL) return ;
		if(dst->ptr == NULL) { header = header->next;b = header->b;continue;}
		if(strncasecmp("accept-encoding", dst->ptr, dst->size) == 0) {
			buffer_get_line(b, dst);
			if(dst->size == 0) {
				in->accept_encoding = _NOCOMPRESS;
			}
			else if(strstr(dst->ptr, "gzip") != NULL) {
				in->accept_encoding = _GZIP;
			}
			else if(strstr(dst->ptr, "deflate") != NULL) {
				in->accept_encoding = _DEFLATE;
			}
		}
		else if(strncasecmp("host", dst->ptr, dst->size) == 0) {
			buffer_get_line(b, dst);
			in->host = (read_buffer *)palloc(p, sizeof(read_buffer));
			in->host->ptr = dst->ptr;
			in->host->size = dst->size;
		}
		else if(strncasecmp("authorization", dst->ptr, dst->size) == 0) {
			buffer_get_line(b, dst);
			in->authorization = (read_buffer *)palloc(p, sizeof(read_buffer));
			in->authorization->ptr = dst->ptr;
			in->authorization->size = dst->size;
			decoded_usr_pwd(con);
		}
		else if(strncasecmp("content-length",dst->ptr, dst->size) == 0) {
			buffer_get_line(b, dst);
			in->content_length = (read_buffer *)pcalloc(p, sizeof(read_buffer));
			in->content_length->ptr = dst->ptr;
			in->content_length->size = dst->size;

		}
		else {
			buffer_get_line(b, dst);
			
		}
	}

	//test_print_header(in);
	return ;
}


int
accept_handler(http_conf *g, http_connect_t *con, struct epoll_event *ev)
{

 	con->in = (request *)request_init(con->p); 

	if(read_header(con) == 0) {
		if(con->in->header == NULL || con->in->header->b == NULL 
			|| con->in->header->b->size <= 0) {

			send_bad_request(con->fd);
			epoll_del_fd(g->epfd, ev);
			close(con->fd);
			pool_destroy(con->p);
			return -1;
		}	
	
		parse_header(con);  
		list_buffer_to_lower(con->in->header);
		con->next_handle = authorized_handle;
		virtual_port_match(g, con);
		epoll_edit_fd(g->epfd, ev, EPOLL_W);
		/*是否需要登录
		*提交方式
		*主页匹配
		*压缩
		*发送数据
		*/
		//http_connect_t *con;
		//con = ev[count].data.ptr;

		con->out = (response *)response_init(con->p);
		while(con->next_handle != NULL && con->next_handle(g, con) == 0){	
		}
		
 		epoll_del_fd(g->epfd, ev);
					
	}

	return 0;
}

int http_send_cgi_body(epoll_cgi_t *cgi) {
	list_buffer *lb;

	lb = cgi->out;
	while(lb != NULL && lb->b != NULL && lb->b->ptr != NULL) {
		write(cgi->con->fd, lb->b->ptr, lb->b->size);
		lb = lb->next;
	}
}

int
parse_cgi(epoll_cgi_t *cgi)
{
	list_buffer *cgi_data, *send;
	buffer *b;
	char *p;
	int count;
	buffer *header, *out, *tmp;

	cgi_data = cgi->cgi_data;
	if(cgi_data == NULL || cgi_data->b == NULL || cgi_data->b->ptr == NULL || cgi_data->b->size == 0) return 0;
	b = cgi_data->b;
	send = (list_buffer *) pcalloc(cgi->con->p, sizeof(list_buffer));
	header = (buffer *) pcalloc(cgi->con->p, sizeof(buffer));

	cgi->con->out->status_code = HTTP_OK;
	p = strstr(b->ptr,"\n\n");
	if(p != NULL && (p-b->ptr) < b->size) {
		buffer_n_to_lower(b, (p-b->ptr)+1);
		header->ptr = b->ptr;
		header->size = (p-b->ptr) + 2;
		
		tmp = (buffer *) pcalloc(cgi->con->p, sizeof(buffer));
		buffer_find_str(header, tmp, "content-type:");
		if(tmp->ptr == NULL) {
			send_bad_gateway(cgi->con->fd);
			return 1;
		}
		p = tmp->ptr + tmp->size;
		while(*p == ' ') p++;

	
		tmp->ptr = p;
		p = strchr(p, '\n');
		
		count = p - tmp->ptr ;
		if(*(p-1) != '\r' ) count++;
		tmp->size = count;

		cgi->con->out->content_type = buffer_create_size(cgi->con->p, count);
		cgi->con->out->content_type->size = count;
		strncpy(cgi->con->out->content_type->ptr, tmp->ptr, count -1);
		cgi->con->out->content_type->ptr[count-1] = 0;

		out = (buffer *)pcalloc(cgi->con->p, sizeof(buffer));
		out->ptr = header->ptr + header->size;
		out->size = b->size - header->size;

		send->b = out;
		send->next = cgi_data->next;
		cgi->out = send; 

		


	}
	

}

int 
cgi_parse_handler(epoll_cgi_t *cgi)
{
	parse_cgi(cgi);
	if(http_send_header(cgi->con) == 0) {
		http_send_cgi_body(cgi);
		return 0;
	}
	return 1;
	
}

int
start_accept(struct http_conf *g)
{
	int count; 
	struct epoll_event ev[MAX_EVENT];
	epoll_extra_data_t *epoll_data;
	
	
	start_web_server(g);
	
	printf("--------------- start server\n--------------");
	while(1){
		count = epoll_wait(g->epfd, ev, MAX_EVENT, -1);
		while(count-- > 0){
			epoll_data = (epoll_data_t *)ev[count].data.ptr;
			if(epoll_data->type  == SERVERFD) {
				int confd =  accept(g->fd, NULL, NULL);
				struct pool_t *p = (struct pool_t *)pool_create();
				http_connect_t * con;
				epoll_extra_data_t *data_ptr;

				data_ptr = (epoll_extra_data_t *)palloc(p, sizeof(epoll_extra_data_t));
				con = (http_connect_t *) palloc(p, sizeof(http_connect_t));//换成初始化函数，
				con->p= p;
				con->fd = confd;
				data_ptr->type = SOCKFD;
				data_ptr->ptr = (void *) con;
				epoll_add_fd(g->epfd, confd, EPOLL_R, (void *)data_ptr);//对epoll data结构指向的结构体重新封装，分websit
	 			//data struct ,  connect  data struct , file data struct , 
			}
			else if((ev[count].events & EPOLLIN)) {
				http_connect_t * con;
				
				con = (http_connect_t *) epoll_data->ptr;
				switch(epoll_data->type) {
					case SOCKFD:
						accept_handler(g, con, ev+count);
	 					break; 
					case CGIFD: {
	 					epoll_cgi_t *cgi = (epoll_cgi_t *)epoll_data->ptr;
						if(read_cgi(cgi) == 0) {
							list_buffer_used_to_zero(cgi->cgi_data);
							if(cgi_parse_handler(cgi) == 0) {
								

							}
							else {
								
							}
							epoll_del_fd(g->epfd, ev);
							pool_destroy(cgi->con->p);
	 	 					close(cgi->con->fd);
						}
						else {
						}
						
		 				break;
	 	 			}
	 	 		}
				

			}
			else if(ev[count].events & EPOLLOUT) {
				
	 	 	}


	 	} 
	} 
}
