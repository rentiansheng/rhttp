#include "http_autoindex.h"

#define strcon(x,y) x y

#define AUTOPAGELEN 20
#define CGI_SUFFIX "cgi"




int file_handle(http_conf_t *conf, http_connect_t *con)
{
	char *p;
	int len = 0 ;
	rkey_t *k;

	p = strchr(con->out->physical_path->ptr, '.');
	if(p != NULL) {
		p++;
		len = strlen(con->out->physical_path->ptr) - (p - con->out->physical_path->ptr);
		
		if(strncmp(p, CGI_SUFFIX, strlen(CGI_SUFFIX)) == 0) {
			con->next_handle = cgi_handle;
			return 0;
		}
		k = conf->mimetype;		

		while(k != NULL) {

			//if(len == strlen(k->name+1) && strncmp(p, k->name+1, len) == 0) {
			if(string_compare_str(k->name, p) == 0) {
				con->out->content_type = buffer_init(con->p);
				string * value = (string *)k->value;
				con->out->content_type->ptr = value->ptr;
				con->out->content_type->size = con->out->content_type->size = value->len;
				break;
			}
			k = k->next;

		}
		if(con->out->content_type == NULL ) {
			con->out->content_type = buffer_init(con->p);
			buffer_append_str(con->p, con->out->content_type, "text/plain", 10);

		}
				
	}

	con->next_handle = http_send;
	
	return 0;
}

int autoindex_handle(http_conf_t *conf, http_connect_t *con)
{
	request * in;
	response *out;
	web_conf_t *web;
	buffer *uri, *tmp;
	struct stat buf;
	int path_len;

	in = con->in;
	out = con->out;
	web = con->web;

	uri = buffer_init(con->p);
	

	/*memset(uri->ptr, 0, path_len);
	uri->used = in->uri->len;
	strncpy(uri->ptr, in->uri->ptr, path_len-1);*/
	buffer_append_str(con->p, uri, in->uri->ptr, in->uri->len);
	buffer_add_str_end(con->p, uri);


	buffer_path_simplify(uri, uri);

	buffer_add_prefix(con->p, uri, web->root->ptr, web->root->len);//len+1，+1用来存字符串结束标识
	buffer_add_str_end(con->p, uri);
	//添加行结束标记

	


	con->next_handle = http_send;

	int strUriLen = uri->used;

	
	//if(chdir(web->root->ptr) == 0 ) {
	if(access(uri->ptr,F_OK|R_OK) == 0) {
		if(stat(uri->ptr, &buf) != 0 ) {
			out->status_code = HTTP_NOT_FOUND;
			return 0;
		}
		
		if(S_ISDIR(buf.st_mode)) {
		
			string * index = web->index_file;
			//添加文件分割符号/，
			if(uri->ptr[uri->used-1] != '/') {
				buffer_append_char(con->p, uri, '/');
			}
			buffer_append_context(con->p, uri, index->ptr, index->len);
			//buffer->size 有问题， used=15， size=30 不是2的幂
			buffer_add_str_end(con->p, uri);
			if(access(uri->ptr, F_OK) == 0 ) {
				/*strcat(uri->ptr, index);

				uri->used += strlen(uri->ptr);*/
				
				//buffer_append_context(con->p, uri ,index->ptr, index->len+1);
				out->status_code = 200;
				out->physical_path = uri;

			} else {
				out->status_code = HTTP_NOT_FOUND;
				return 0;
			}

			
			
		}else if(S_ISREG(buf.st_mode)) {
			out->physical_path = uri;
		}
	}else {
		out->status_code = HTTP_NOT_FOUND;
		
		return  0;
	}
	con->next_handle =  file_handle;
	return 0; 
}

