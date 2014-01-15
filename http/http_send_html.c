#include <http_send_html.h>

int 
produce_http_header(http_connect_t *con) 
{
	char *ptr;
	read_buffer *rb; 
	response *out = con->out;

	rb= palloc(con->p, sizeof(read_buffer));
	ptr = rb->ptr = palloc(con->p, 1024);
	rb->size = 1024;

	sprintf(ptr, "HTTP/1.1 %d", out->status_code);
	switch(out->status_code) {
		case HTTP_OK:
			sprintf(ptr, "%s OK\r\n", ptr);
			break;
		default:
			sprintf(ptr, "%s HTTP_UNDEFINED\r\n", ptr);
			break;
	}
	strcat(ptr, "Connection: Close\r\nContent-Type: ");
	strncat(ptr, out->content_type->ptr, out->content_type->size);
	sprintf(ptr, "%s\r\n", ptr);
	if(out->content_encoding != _NOCOMPRESS ){
		if(out->content_encoding == _GZIP) {
			sprintf(ptr, "%sAccept-Encoding: gzip\r\n", ptr);
		}
		else if(out->content_encoding == _DEFLATE) {
			sprintf(ptr, "%sAccept-Encoding: deflate\r\n", ptr);
		} 
	}
	if(_Server != NULL) sprintf(ptr, "%sServer: %s\r\n", ptr, _Server);
	sprintf(ptr, "%sWWW-Authentlcate:Basic realm\"%s\"\r\n\r\n", ptr, _Auth_desc);

	if(strlen(ptr) == write(con->fd, ptr, strlen(ptr)))
		return 0;
	else
		return 1;
}

int 
http_send_header(http_connect_t *con)
{
	response *out;
	int step_code = UNDEFINED;

	out = con->out;
	if(out == NULL) {
		out = response_init(con->p);
	}

	switch(out->status_code) {
		case HTTP_BAD_REQUEST:
			break;
		case HTTP_UNAUTHORIZED:
			if(send_unauthorized(con->fd) >= 0) {
				step_code = DONE;
			}
			break;
		case HTTP_NOT_FOUND:
			if(send_not_find(con->fd) >= 0)	{
				step_code = DONE;
			}
			break;
		case HTTP_BAD_GATEWAY:
			if( send_bad_gateway(con->fd) >= 0) {
				step_code =  DONE;
			}
			break;
		default:
			if(produce_http_header(con) == 0) {
					step_code = OK;
			}
			else {
				step_code = CANCEL;
			}
	 		break;
	}

	if(step_code == UNDEFINED) {
		step_code = CANCEL;
	}
	
	return step_code;
}

int 
http_send_body(http_connect_t *con)
{
	struct stat sf;

	if(!lstat(con->out->physical_path->ptr, &sf) == 0 ) {
		int fd  = open(con->out->physical_path->ptr,  O_RDONLY);
		void *start = mmap(NULL, sf.st_size, PROT_READ, MAP_SHARED, fd, 0);
		write(con->fd, start, sf.st_size);

		munmap(start, sf.st_size);
		close(fd);

		return 0;
	}

	return 1;
}
