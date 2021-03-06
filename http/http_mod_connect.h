/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */

#ifndef _HTTP_MOD_CONNECT_H_
#define _HTTP_MOD_CONNECT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "base.h"
#include "linux_epoll.h"


#define MAX_CONNECT 10000
#define MAX_EVENT 800

int socket_listen(char *ip, unsigned short int port);

int start_web_server(http_conf_t *conf);


#endif

