#include "http_mod_cgi.h"


void
add_envp(pool_t *p, cgi_ev_t * cgiev, char *left, char *right)
{
	char *p;
	int len;

	len = strlen(left) + strlen(right) + 2;
	p = (char *) palloc(p, len);
	sprintf(p, "%s=%s", left, right);
	
	cgiev->ev[cgiev->count++] = p;
	
}



int
cgi_handle(http_conf *g, http_connect_t *con)
{
	con->next_handle = NULL;

	cgi_ev_t *cgi;
	char *argv[2] = {0};

	cgi = (cgi_ev_t *)pcalloc(con->p, sizeof(cgi_ev_t));

	cgi->count = 0;
	if(con->in->http_method == _GET) {
		add_envp(con->p, cgi, , "get");
	}
	return 0;	
}
