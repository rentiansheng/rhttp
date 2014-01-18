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
#include "unistd.h"

int autoindex_handle(http_conf *g, http_connect_t *con);

#endif
