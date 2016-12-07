/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */

#ifndef _HTTP_MOD_AUTHORIZED_H_
#define _HTTP_MO_AUTHORIZED_H_
#include <string.h>
#include "http_mod_connect.h"
#include "base.h"
#include "buffer.h"
#include "http_send_page.h"
#include "http_request.h"
#include "http_autoindex.h"
#include "http_send_html.h"
#include "str.h"

#define AUTH_PAGES "noauth.html"

int authorized_handle(http_conf_t *conf, http_connect_t *con);
#endif
