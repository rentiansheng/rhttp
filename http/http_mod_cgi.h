/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#ifndef _HTTP_MOD_CGI_H_
#define _HTTP_MOD_CGI_H_
#include "base.h"
#include "http_request.h"
#include "http_mod_connect.h"
#include "errno.h"
#include "string.h"

int cgi_handle(http_conf_t *conf, http_connect_t *con);

extern char **environ;

#endif
