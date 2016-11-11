/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */

#ifndef _HTTP_AUTOINDEX_H_
#define _HTTP_AUTOINDEX_H_H

#include "base.h"
#include "http_request.h"
#include "pool.h"
#include "buffer.h"
#include "sys/stat.h"
#include "http_mod_cgi.h"
#include "unistd.h"
#include "str.h"

int autoindex_handle(http_conf_t *g, http_connect_t *con);

#endif
