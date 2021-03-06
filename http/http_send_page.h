/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
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

int send_bad_request(int fd);
#endif
