#include "http_virtual_port.h"


void 
virtual_port_match(http_conf *g, http_connect_t *con)
{
	int port ;
	read_buffer *rbport;
	read_buffer *host;
	web_conf *web;


	con->web = NULL;
	rbport = (read_buffer *)palloc(con->p, sizeof(read_buffer));
	host = con->in->host;
	web = g->web;
	//一下是为了支持多个端口绑定
	read_buffer_get_line_split(host, rbport, ':');

	if(rbport->size > 0 && rbport->ptr != NULL){
		port = atoi(rbport->ptr);
	}

	  while(web != NULL) {
		  if(web->server == NULL) {
			  if(read_buffer_compare_str(host, "127.0.0.1") == 0
				  || read_buffer_compare_str(host, "localhost") == 0) {
				  con->web = web;
			  }
		  }
		  else if(read_buffer_compare_str(host, web->server) == 0) {
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
