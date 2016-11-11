#include "http_mod_authorized.h"

int authorized_handle(http_conf_t *conf, http_connect_t *con)
{
	int result;
	char usr[20];
	char pwd[40];

	usr[0] =  pwd[0] = 0;
	if(con->in->user != NULL  &&  con->in->pwd != NULL) {
		string_copy_n_to_str(con->in->user, usr, con->in->user->len);
		string_copy_n_to_str(con->in->pwd, pwd, con->in->user->len);
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
