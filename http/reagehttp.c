/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 * Mail: reage521@gmail.com
 */
#include <stdio.h>
#include "config.h"
#include "base.h"
#include "http_deamon.h"
#include "http_request.h" 
#include "pool.h"


void signal_exit(int no) 
{
	kill(0, SIGTERM);
	exit(0);
}

int main(int argc, char *argv[]) {
	char conf_path[255] = "reage.conf";
	int err ;

	pool_t *p = (pool_t *) pool_create();

	http_conf_t * conf = (http_conf_t *)pcalloc(p, sizeof(http_conf_t));
	conf->p = p;

 
	signal(SIGTERM, signal_exit); //* kill 
	signal(SIGINT, signal_exit); //按下Ctrl-C得到的结果 
	signal(SIGQUIT, signal_exit);  //按下Ctrl-得到的结果 


	err = config_init(conf_path, conf);
	if( err ) {
		printf("error config\n ");
	} else {
		sprintf(argv[0], "rhttp master                                       ");
		http_daemon(conf);
		sprintf(argv[0], "rhttp work                                         ");
		start_accept(conf);	
	}
}


