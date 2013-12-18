#include "http_mod_authorized.h"

int
authorized_handle(http_conf *g, http_connect_t *con)
{
	so_t so;
	int result;
	char usr[20];
	char pwd[40];

	strcpy(so->path, "authorized.so");
	strcpy(so->function, "handle");

	if(so->f == NULL) {
		con->out.status_code = HTTP_BAD_GATEWAY;
		return 3;// 501
	}


	read_buffer_to_str_n(con->in->user, usr, sizeof(usr));
	read_buffer_to_str_n(con->in->user, pwd, sizeof(pwd));
	
	if(strlen(usr) == 0 || strlen(pwd) == 0) {
		con->out.status_code = HTTP_UNAUTHORIZED;
		return 2;
	}

	result = so->f(usr, pwd);
	if(result == 0) con->next_handle = auto_indexfile;

	return result;
}
