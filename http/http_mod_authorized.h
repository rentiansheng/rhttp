/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */

#ifndef _HTTP_MOD_AUTHORIZED_H_
#define _HTTP_MO_AUTHORIZED_H_
#include <so_dlfcn.h>
#include <string.h>
#include "http_mod_connect.h"
#include "base.h"

int authorized_handle(http_conf *g, http_connect_t *con);
#endif
