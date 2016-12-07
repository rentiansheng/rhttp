#include "http_virtual_port.h"


void virtual_port_match(http_conf_t *conf, http_connect_t *con)
{
	int port ;
	string *rbport;
	string *host;
	web_conf_t *web;


	con->web = NULL;
	rbport = string_init(conf->p);
	host = string_init(conf->p);
	web = conf->web;
	//一下是为了支持多个端口绑定
	string_split_kv(con->in->host, host, rbport, ':');

	if(rbport != NULL && rbport->len > 0 && rbport->ptr != NULL){
		port = atoi(rbport->ptr);
	}




	  while(web != NULL) {
		  con->web = web;
		 if(string_compare(host, web->name) == 0) {
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
