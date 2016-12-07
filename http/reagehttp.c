/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */
#include <stdio.h>
#include "config.h"
#include "base.h"
#include "http_deamon.h"
#include "http_request.h" 
#include "pool.h"

int main(int argc, char *argv[]) {
	char conf_path[255] = "reage.conf";
	int err ;

	pool_t *p = (pool_t *) pool_create();

	http_conf_t * conf = (http_conf_t *)pcalloc(p, sizeof(http_conf_t));
	conf->p = p;



	err = config_init(conf_path, conf);
	if( err ) {
		printf("error config\n ");
	} else {
		http_daemon(conf);
		start_accept(conf);	
	}
}
