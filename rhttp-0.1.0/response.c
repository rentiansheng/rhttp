#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>

#include "mod_compress.h"
#include "key.h"
#include "memory.h"




void send_http_not_web(int conn_socket)
{ 
	char *send_buf = "HTTP/1.1 203 Non-Authoritative Information\r\nServer: Reage webserver\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<!DOCTYPE html><html><head><title>203 Non-Authoritative Information</title></head><body><h1>R203 Non-Authoritative Information</h1><hr/>reage web server 1.0.1 </body></html>\r\n\r\n";
	write(conn_socket, send_buf, strlen(send_buf));
}

void send_http_not_find(int conn_socket)
{ 
	char *send_buf = "HTTP/1.1 404 Not Found\r\nServer: Reage webserver\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1><hr/>reage web server 1.0.1 </body></html>\r\n\r\n";
	write(conn_socket, send_buf, strlen(send_buf));
}

void send_http_not_Forbidden(int conn_socket)
{ 
	char *send_buf = "HTTP/1.1 403 Forbidden\r\nServer: Reage webserver\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<!DOCTYPE html><html><head><title>403 Forbidden</title></head><body><h1>404 Not Found</h1><hr/>reage web server 1.0.1 </body></html>\r\n\r\n";
	write(conn_socket, send_buf, strlen(send_buf));
}


void send_filter(request * client_request, const globle_conf *g, response * rp){
	read_buffer *page = (read_buffer *)client_request->pages;
	read_buffer *type = rp->content_type = (buffer *)read_buffer_init();  
	char *p;
	char *pg = r_malloc(page->size +1);
	strncpy(pg, page->ptr, page->size);
	pg[page->size] = 0;

	p = strchr(pg, '.');
	type->ptr= key_indexof_key(g->mimetype, p);
	if(NULL == type->ptr){
		type->size = 0;
		rp->status_code = 404;
	}
	else {
		type->size = strlen(type->ptr);
	}
	free(pg);
}

void send_header(response *r, int sockd){
	buffer *head = buffer_init_string( "HTTP/1.1 200 OK\r\nServer: Reage webserver\r\n");
	switch(r->content_encoding){
		case 1:
			buffer_append_string(head, "Content-Encoding: deflate\r\n");
			break;
		case 2:
			buffer_append_string(head, "Content-Encoding: gzip\r\n");
			break;
	}
	buffer_append_string(head, "Content-Type: ");
	
	read_buffer *rb = r->content_type;
	buffer_append_string(head, rb->ptr);
	buffer_append_string(head, "\r\nConnection: close\r\n\r\n");


	write(sockd, head->ptr, head->used -1);
	free_read_buffer(rb);
	buffer_free(head);
}

void send_data(response *r, request * client_request){

	buffer *file;	
	struct stat sf;

	file = (buffer *)client_request->uri;

	if(!lstat(file->ptr + 1, &sf)){
		if(S_ISREG(sf.st_mode)){
			int f = f = open(file->ptr + 1, O_RDONLY);
			if(0 > f){
				//403 Forbidden
				send_http_not_Forbidden(client_request->sockfd);
				return;
			}
			void *start = mmap(NULL, sf.st_size, PROT_READ, MAP_SHARED, f, 0);
			r->len = sf.st_size;
			r->start = start;
			r->out = (buffer *)buffer_init();
	

			hander_compress(r, client_request->compress);
			buffer *out = (buffer *)r->out;
	
			send_header(r, client_request->sockfd);


			write(client_request->sockfd, out->ptr, out->used );
	
			munmap((void *)start, sf.st_size);
			free(out->ptr);
		}
		else{
			//404	
			send_http_not_find(client_request->sockfd);	
		}

	}
	else{
		//404	
		send_http_not_find(client_request->sockfd);	
	}
	
}

void response_handler(request * client_request, const globle_conf *g){

	buffer *file;	
	read_buffer *rb = (read_buffer *)client_request->pages;
	struct stat sf;
	response r;
	
	if(NULL  == client_request->web){
		 //203服务器已成功处理了请求，但返回的实体头部元信息不是在原始服务器上有效的确定集合，
		send_http_not_web(client_request->sockfd);
	}
	else if(0 >= rb->size){
		//404	
		send_http_not_find(client_request->sockfd);	
	}
	else{
	
		send_filter(client_request, g, &r);
		if(404 == r.status_code ){
			send_http_not_find(client_request->sockfd);
		}	
		else{
			
			send_data(&r, client_request);
		}
	
	}
	buffer *header = client_request->header;
	//free(header->ptr);
	//free(client_request);

}

