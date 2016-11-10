#include "http_mod_authorized.h"

int
authorized_handle(http_conf *g, http_connect_t *con)
{
	int result;
	char usr[20];
	char pwd[40];

	usr[0] =  pwd[0] = 0;
	if(con->in->user != NULL  &&  con->in->pwd != NULL) {
		read_buffer_to_str_n(con->in->user, usr, con->in->user->size);
		read_buffer_to_str_n(con->in->pwd, pwd, con->in->user->size);
	}

	/* 身份验证*/
	result = 0;
	con->next_handle = http_send;
	if(strcmp(usr,"1234") == 0 && strcmp(pwd, "1234") == 0) {
		con->next_handle = autoindex_handle;
	}
	else  {
		con->out->status_code = HTTP_UNAUTHORIZED;
		con->out->physical_path = AUTH_PAGES;
	}

	return 0;
}
