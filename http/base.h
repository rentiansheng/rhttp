/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */
#ifndef _HTTP_BASE_H_
#define _HTTP_BASE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "pool.h"

#define OK 0
#define FILE_NO_EXIST 1
#define FILE_NO_ACCESS 2
#define CANCEL 3
#define DONE 4
#define MEMERROR -1
#define UNDEFINED   -144

typedef enum {
	READ_HEAD,
	HEAD_PRASE,
	HANDLE,
	COMPRESS,
	WRITE,
}step;

typedef enum {
	t_char,
	t_uchar,
	t_int,
	t_uint,
	t_long,
}value_type;

typedef struct key {
	char *name;
	void *value;
	value_type type;
	struct key *next;
}key;


typedef struct fileinfo {
	read_buffer * name;
	size_t len;
	int fd;
	void *start;
}fileinfo_t;

typedef struct web_conf {
	char *root;
	char *index_file;
	char *err404;
	char *server;
	int fd;
	struct web_conf *next;
}web_conf;

typedef struct http_conf {
	int web_count;
	int port;
	int epfd;
	int fd;
	key *mimetype;
	web_conf *web;
}http_conf;



typedef struct http_connect{
	struct request *in;
	struct response *out;
	struct web_conf *web;
	struct pool_t *p;
	int fd;
	int (*next_handle)(http_conf *g, struct  http_connect *con);
} http_connect_t;


#define _Server "Rhttp" 
#define _Auth_desc "Rhttp test" 
#define _Version "0.2.0" 



#endif
