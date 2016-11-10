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

void 
http_accept_request_socket(struct http_conf * g) {
	int confd  ;
	struct sockaddr addr;
	struct sockaddr_in addrIn;
	socklen_t addLen = sizeof(struct sockaddr );
	while( (confd =  accept(g->fd, &addr, &addLen)) && confd > 0) {
		pool_t *p = (pool_t *)pool_create();
		http_connect_t * con;
		epoll_extra_data_t *data_ptr;

		addrIn =  *((struct sockaddr_in *) &addr);
		data_ptr = (epoll_extra_data_t *)palloc(p, sizeof(epoll_extra_data_t));
		con = (http_connect_t *) palloc(p, sizeof(http_connect_t));//换成初始化函数，
		con->p= p;
		con->fd = confd;
		con->in = (request *)request_init(p);
		con->out = (response *)response_init(p);
		/*char *ip  = NULL;
		if(addrIn.sin_addr.s_addr) {
			ip = inet_ntoa(addrIn.sin_addr);
		}

		if(ip) {
			//con->in->clientIp = (string *) string_init_from_str(p, ip, strlen(ip));
		}*/

		con->next_handle = accept_handler;
		data_ptr->type = SOCKFD;
		data_ptr->ptr = (void *) con;
		epoll_add_fd(g->epfd, confd, EPOLL_R, (void *)data_ptr);//对epoll data结构指向的结构体重新封装，分websit
		//data struct ,  connect  data struct , file data struct ,
	}
}

int
start_accept(struct http_conf *g)
{
	int count; 
	struct epoll_event ev[MAX_EVENT];
	epoll_extra_data_t *epoll_data;
	struct epoll_event * evfd;

	int evIndex ;

	
	
	start_web_server(g);
	
	printf("--------------- start server\n--------------");
	while(1){
		count = epoll_wait(g->epfd, ev, MAX_EVENT, -1);
		http_accept_request_socket(g);
		if(count < 0) { count = 0;}
		
		for(evIndex = 0; evIndex < count; evIndex++) {
			evfd = ev + evIndex;
			epoll_data = (epoll_data_t *)evfd->data.ptr;
			if(epoll_data->type  != SERVERFD && (evfd->events & EPOLLIN)) {
				http_connect_t * con;
				
				con = (http_connect_t *) epoll_data->ptr;
				switch(epoll_data->type) {
					case SOCKFD:
						accept_handler(g, con, evfd);
	 					break; 
					case CGIFD: {
	 					epoll_cgi_t *cgi = (epoll_cgi_t *)epoll_data->ptr;
						if(read_cgi(cgi) == 0) {
							list_buffer_used_to_zero(cgi->cgi_data);
							if(cgi_parse_handler(cgi) == 0) {
								

							}
							else {
								
							}
							epoll_del_fd(g->epfd, evfd);
							pool_destroy(cgi->con->p);
	 	 					close(cgi->con->fd);
						}
						else {
						}
						
		 				break;
	 	 			}
	 	 		}
				

			}
			else if(evfd->events & EPOLLOUT) {
				
	 	 	}else if(evfd->events & EPOLLRDHUP) {
				http_connect_t * con;
				epoll_data = (epoll_data_t *)evfd->data.ptr;

				con = (http_connect_t *) epoll_data->ptr;	
				if(con->in != NULL) {
					epoll_edit_fd(g->epfd, evfd, EPOLL_W);
					
					pool_destroy(con->p);
					close(con->fd);

				}
				con->next_handle = NULL;
				epoll_del_fd(g->epfd, evfd);

		
			}


	 	} 

	} 
}
