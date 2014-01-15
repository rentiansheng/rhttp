#include "http_mod_authorized.h"

int
authorized_handle(http_conf *g, http_connect_t *con)
{
	int result;
	char usr[20];
	char pwd[40];

	usr[0] =  pwd[0] = 0;
	if(con->in->user != NULL  &&  con->in->pwd != NULL) {
		read_buffer_to_str_n(con->in->user, usr, sizeof(usr));
		read_buffer_to_str_n(con->in->pwd, pwd, sizeof(pwd));
	}



	if(strlen(usr) == 0 || strlen(pwd) == 0) {
		con->out->status_code = HTTP_UNAUTHORIZED;
		return 2;
	}

	/* 身份验证*/
	result = 0;
	if(result == 0) {
		con->out->physical_path = AUTH_PAGES;
		return 0;
	}

	return 0;
}
