#include <stdio.h>
#include "config.h"
#include "base.h"
#include "http_request.h" 

int 
main(int argc, char *argv[]) {
	char conf_path[255] = "reage.conf";
	int err ;
	struct http_conf conf;

	conf.web = NULL;
	conf.web_count = 0;
	conf.mimetype = NULL;
	err = config_init(conf_path, &conf);
	if( err ) {
		printf("error config\n ");
	} else {
		http_daemon(&conf)
		start_accept(&conf);	
	}
}
