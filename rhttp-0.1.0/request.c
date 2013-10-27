/*
*主要实现功能， 处理接受的链接的信息，解析请求客户端请求。
* 作者：Reage
* blog:http://blog.csdn.net/rentiansheng
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "memory.h"
#include <unistd.h>
#include <string.h>

#include "buffer.h"
#include "base.h"
#include "key.h"

/*get web browser request header;
*得到浏览器的请求头部
*
*/
static void get_request_header(request * client_request){
	char buf[2000];
	char *p;
	int count ;

	buf[1999]=  0;	
	count = read( client_request->sockfd, buf, 1999);
	client_request->header = (struct buffer *) buffer_init_string(buf);
	buffer_to_lower((buffer*)client_request->header);
	return;
}



/*get uri、methed、 http version
*得到请求的url、请求的方式、http的版本
*/
static void handler_uri_mothed(request * client_request){
	
	char *p;	
	int count;
	read_buffer *rb ;
	buffer * tmp_buffer = (buffer *) client_request->header;


	p = (char *)strchr(tmp_buffer->ptr, ' ');	

	count = p - tmp_buffer->ptr;
	if(0 == strncmp("get", tmp_buffer->ptr, count)){
		client_request->http_method = get; 
	}else if( 0 == strncmp("post", tmp_buffer->ptr, count)){
		client_request->http_method = post;	
	}else{
		client_request->http_method = none;
		/* send error page*/
		return;
	}

	rb = read_buffer_init();
	
	client_request->uri = (struct buffer *)buffer_init();
	read_buffer_get_string(p , rb);
	if(NULL != p){
		buffer_copy_string_len((buffer *)client_request->uri, rb->ptr, rb->size);
	}
	else{
		buffer_copy_string_len((buffer *)client_request->uri, "/", 1);
	}
	p += rb->size;


	client_request->http_version = (struct read_buffer * )read_buffer_init();
	read_buffer_get_string(p, (read_buffer *)client_request->http_version);

	free(rb);
	return;
}

/*Browser support obtained compression method  and host
*得到浏览器的支持的压缩方式和请求的网站域名
*/
static void get_host_compress(request * client_request){

	buffer * tmp_buffer = (buffer *) client_request->header;
	char *p = strstr( tmp_buffer->ptr, "host:");

	client_request->host =(struct read_buffer *)read_buffer_init();
	if(NULL != p){
		read_buffer_get_line(p+5, (read_buffer *)client_request->host);
	}	
	p = strstr( tmp_buffer->ptr, "accept-encoding:");
	if(NULL != p){
		client_request->compress = (struct read_buffer *)read_buffer_init();
		read_buffer_get_line(p+16, (read_buffer *)client_request->compress);
	}		
}


/*通过host得到网站对应在服务器上的物理路径。
*/
void host_to_physical(request * client_request, const globle_conf *g){

	int i = g->web_count;
	read_buffer *rb = (read_buffer *)client_request->host;
	client_request->web = NULL;
	struct web_conf * web = g->web_config;
	if(NULL == web)return;
	while(web){
		if(strlen(web->server) == rb->size && 0 == strncasecmp(web->server, rb->ptr, rb->size)){
			client_request->web = 	web;
			return ;
		}
		web = web->next;
	}
}


void default_pages(request * client_request){

	char  *pre, *p = NULL;
	int count = 0;
	char *fname;
	struct stat sf;
	buffer *b = (buffer *)client_request->uri;
	struct web_conf * web = client_request->web ;
	
	if(NULL == web){
		client_request->pages = (struct read_buffer *)read_buffer_init();	
		return;
	}
	p = b->ptr + b->used -1;
	while('/' != *p ) p --;
	pre = strchr(p, '.');
	
	if(NULL == pre ){
		
		if(chdir(web->root)){p = NULL , count = 0;}		
		buffer *uri = (buffer *)client_request->uri;
		if(chdir(uri->ptr+1)){p = NULL , count = 0;}

		p = web->index_file;
		fname = (char *)r_malloc(255*sizeof(char));
		while(NULL != p){
			pre = strchr(p, ',');
			if(NULL == pre){
				if(!lstat(p, &sf)){
					if(S_ISREG(sf.st_mode)){
						count = strlen(p);
						buffer_append_string(uri, p);
					}			
				}
				break;
			}
			else{
				memset(fname, 0, 255);
				strncpy(fname, p, pre - p);
				if(!lstat(fname, &sf)){
					if(S_ISREG(sf.st_mode)){
						count = pre - p;
						buffer_append_string(uri, fname);
						break;
					}			
				}
				
			}
			p = pre + 1;
		}
		
	}
	else{
		p ++;
		count = strlen(p);
		
	}
	chdir(web->root);
	client_request->pages = (struct read_buffer *)read_buffer_init_str(p,count);
}




void handler_request(request * client_request, const globle_conf *g){


	
	get_request_header(client_request);

	handler_uri_mothed(client_request);

	
	get_host_compress(client_request);

	buffer_path_simplify((buffer *)client_request->uri, (buffer *)client_request->uri);

	host_to_physical(client_request,  g);

	default_pages(client_request);	

}




