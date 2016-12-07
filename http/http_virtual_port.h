/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 * desc:
 */
#ifndef _HTTP_VIRTUAL_PORT_H_
#define _HTTP_VIRTUAL_PORT_H_
#include "buffer.h"
#include "str.h"
#include "base.h"
#include "http_mod_connect.h"
#include "http_request.h"

void virtual_port_match(http_conf_t *conf, http_connect_t *con);

#endif
