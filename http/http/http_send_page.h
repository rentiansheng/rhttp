/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */

#ifndef _HTTP_SEND_PAGE_H_
#define _HTTP_SEND_PAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"

int send_unauthorized(int fd);

int send_not_find(int fd);

int send_bad_gateway(int fd);

#endif
