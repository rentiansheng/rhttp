/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */
#ifndef _HTTP_SEND_HTML_H_INCLUDE
#define _HTTP_SEND_HTML_H_INCLUDE

#include "base.h"
#include "http_request.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "str.h"


int produce_http_header(http_connect_t *con);

int http_send_header(http_connect_t *con);

int http_send_body(http_connect_t *con);

int http_send(http_conf *g, http_connect_t *con);


#endif
