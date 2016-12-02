#include "http_mod_authorized.h"

int authorized_handle(http_conf_t *conf, http_connect_t *con)
{
	int result;
	//char usr[20];
	//char pwd[40];

	//usr[0] =  pwd[0] = 0;
	string *usr, *pwd;

	if((con->web->auth_usr != NULL && con->web->auth_usr->len > 0) || (con->web->auth_pwd != NULL &&con->web->auth_pwd->len > 0)) {
		usr = con->web->auth_usr;
		pwd = con->web->auth_pwd;
	} else {
		usr = conf->auth_usr;
		pwd = conf->auth_pwd;
	}


	if(usr == NULL && pwd == NULL) {
		con->next_handle = autoindex_handle;
		return 0;
	} else if(con->in->user != NULL  &&  con->in->pwd != NULL) {
		//如果需要auth，但是如果没有授权信息
	}
	/*if(con->in->user != NULL  &&  con->in->pwd != NULL) {
		string_copy_n_to_str(con->in->user, usr, con->in->user->len);
		string_copy_n_to_str(con->in->pwd, pwd, con->in->user->len);
	}*/

	/* 身份验证*/
	result = 0;
	
	if(string_compare(usr, con->in->user) == 0 && string_compare(pwd, con->in->pwd) == 0) {
		con->next_handle = autoindex_handle;
	}
	else  {
		con->out->status_code = HTTP_UNAUTHORIZED;
		con->out->physical_path = AUTH_PAGES;
		con->next_handle = http_send;
	}

	return 0;
}
