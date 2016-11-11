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

			if(len == strlen(k->name+1) && strncmp(p, k->name+1, len) == 0) {
				con->out->content_type = buffer_init(con->p);
				con->out->content_type->ptr = k->value;
				con->out->content_type->size = con->out->content_type->size = strlen(k->value);
				break;
			}
			k = k->next;

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
	path_len = in->uri->len + 1;
	uri = buffer_create_size(con->p, path_len + AUTOPAGELEN);

	memset(uri->ptr, 0, path_len);
	uri->used = in->uri->len;
	strncpy(uri->ptr, in->uri->ptr, path_len-1);

	buffer_path_simplify(uri, uri);


	con->next_handle = http_send;
	if(chdir(web->root) == 0 ) {
		if(strlen(uri->ptr)>1 && stat(uri->ptr+1, &buf) != 0 ) {
			out->status_code = HTTP_NOT_FOUND;
			return 0;
		}
		
		if(uri->used ==  1 || S_ISDIR(buf.st_mode)) {
			if(strlen(uri->ptr)> 1) {
				if(chdir(uri->ptr+1)) {
					out->status_code = HTTP_NOT_FOUND;
					return 0;
				}
			}
			string * index = web->index_file;
			if(access(index->ptr, F_OK) == 0 ) {
				strcat(uri->ptr, index);
				uri->used += strlen(uri->ptr);
				out->physical_path = uri;
				chdir(web->root);

			} else {
				out->status_code = HTTP_NOT_FOUND;
				return 0;
			}

			
			
		}else if(S_ISREG(buf.st_mode)) {
			if(access(uri->ptr+1,F_OK|R_OK) == 0) {
				out->physical_path = uri;
			}else {
				out->status_code = HTTP_NOT_FOUND;
				return 0;
			}  
			
		}
	}else {
		out->status_code = HTTP_NOT_FOUND;
		
		return  0;
	}
	con->next_handle =  file_handle;
	return 0; 
}

