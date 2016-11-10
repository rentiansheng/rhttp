/*
 * Copyright (C) Reage
 * blog:http://www.ireage.ccom
 */
#ifndef _HTTP_DAEMON_H_
#define _HTTP_DAEMON_H_
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include <pwd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>



#include "base.h"

int http_daemon(http_conf * conf);


#endif