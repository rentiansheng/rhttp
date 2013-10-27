#include <stdio.h>
#include "base.h"
#include "config.h"
#include "epoll.h"

int main()
{
	struct globle_conf g;
	g.web_config = 	NULL;
	g.web_count = 0;
	config_init("reage.conf", &g);
	handler_socket(&g);
	return 0;
}
