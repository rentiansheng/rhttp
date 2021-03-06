
/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */


#ifndef _HTTP_REQUEST_H_INCLUDED_
#define _HTTP_REQUEST_H_INCLUDED_
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "buffer.h"
#include "base.h"
#include "http_mod_connect.h"
#include "http_virtual_port.h"
#include "base64.h"
#include "http_mod_authorized.h"
#include "http_send_page.h"
#include "http_send_html.h"
#include "http_autoindex.h"
#include "linux_epoll.h"
#include "http_request_header.h"




int start_accept(http_conf_t *conf);

response * response_init(pool_t *p);
request * request_init(pool_t *p);

#endif
