#include "http_virtual_port.h"


void 
virtual_port_match(http_conf *g, http_connect_t *con)
{
	int port ;
	string *rbport;
	string *host;
	web_conf *web;


	con->web = NULL;
	rbport = (string *)palloc(con->p, sizeof(string));
	host = con->in->host;
	web = g->web;
	//一下是为了支持多个端口绑定
	string_get_line_split(host, rbport, ':');

	if(rbport != NULL && rbport->len > 0 && rbport->ptr != NULL){
		port = atoi(rbport->ptr);
	}

	con->web = web;

	  while(web != NULL) {
		  if(web->server == NULL) {
			  con->web = web;
		  }
		  else if(string_compare_str(host, web->server) == 0) {
			  con->web = web;
			  return;
		  }
		  /*下面用于多端口绑定的
		   *else if(port == web->port) {
		   *	con->web = web;
		   *	return;
		   *}
		   */
		  
		  
		  web = web->next;
	  }
	
	return ;
}
