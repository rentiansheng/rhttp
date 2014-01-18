#include "http_autoindex.h"

#define strcon(x,y) x y

#define AUTOPAGELEN 20




int 
file_handle(http_conf *g, http_connect_t *con)
{
	char *p;
	int len = 0 ;
	key *k;

	p = strchr(con->out->physical_path->ptr, '.');
	if(p != NULL) {
		p++;
		len = strlen(con->out->physical_path->ptr) - (p - con->out->physical_path->ptr);
		k = g->mimetype;

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

	con->next_handle = NULL;
	
	return 0;
}

int
autoindex_handle(http_conf *g, http_connect_t *con)
{
	request * in;
	response *out;
	web_conf *web;
	buffer *uri, *tmp;
	struct stat buf;
	int path_len;

	in = con->in;
	out = con->out;
	web = con->web;
	path_len = in->uri->size + 1;
	uri = buffer_create_size(con->p, path_len + AUTOPAGELEN);

	memset(uri->ptr, 0, path_len);
	uri->used = in->uri->size;
	strncpy(uri->ptr, in->uri->ptr, path_len-1);

	buffer_path_simplify(uri, uri);



	if(chdir(web->root) == 0 ) {
		if(strlen(uri->ptr)>1 && stat(uri->ptr+1, &buf) != 0 ) {
			out->status_code = HTTP_NOT_FOUND;
			return 1;
		}
		
		if(uri->used ==  1 || S_ISDIR(buf.st_mode)) {
			int i;
			char *index;
			if(strlen(uri->ptr)> 1)chdir(uri->ptr+1);
			index = web->index_file;
			for(i = 0; i < web->index_count; i++) {
				if(access(index, F_OK) == 0 || access(index, R_OK) == 0) {
					strcat(uri->ptr, index);
					uri->used += strlen(uri->ptr);
					out->physical_path = uri;
		 			break;
				}
		 		index = index + strlen(index);
			}
			chdir(web->root);

			if( i == web->index_count) {
				out->status_code = HTTP_NOT_FOUND;
				return 1;
		 	}
		}
		else if(S_ISREG(buf.st_mode)) {
			if(access(uri->ptr+1,F_OK|R_OK) == 0) {
				out->physical_path = uri;
			}
			
		}
		else {
			out->status_code = HTTP_NOT_FOUND;
			return 1;
		}  
	}
	else {
		out->status_code = HTTP_NOT_FOUND;
		return  1;
	}
	con->next_handle =  file_handle;
	return 0; 
}

