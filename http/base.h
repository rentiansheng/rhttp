/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#ifndef _HTTP_BASE_H_
#define _HTTP_BASE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "pool.h"
#include "str.h"
#include "hash.h"



#define FILE_NO_EXIST 1
#define FILE_NO_ACCESS 2

#define OK 0
#define CONTINUE 1
#define CANCEL 3
#define DONE 4

#define MEMERROR -1
#define UNDEFINED   -144


#define HTTP_OK 200

#define HTTP_BAD_REQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_NOT_FOUND 404
#define HTTP_REQUEST_TIMEOUT 408
#define HTTP_NOT_IMPLEMENTED 501
#define HTTP_BAD_GATEWAY 502
#define HTTP_UNDEFINED 544


#define MAX_CONNECT 10000
#define EPOLL_W 1
#define EPOLL_R 2
#define SERVERFD 1
#define SOCKFD 2
#define CGIFD 3

typedef enum{
	_GET,
	_POST,
	_NONE
}http_method_t;

typedef enum {
	_NOCOMPRESS, 
	_GZIP,
	_DEFLATE
}COMPRESS_TYPE;



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
	t_string,
}value_type;

typedef struct rkey {
	string *name;
	void *value;
	value_type type;
	struct rkey_t *next;
}rkey_t;


typedef struct fileinfo {
	string * name;
	size_t len;
	int fd;
	void *start;
}fileinfo_t;

typedef struct web_conf {
	string *root;
	string *index_file;
	string *err404;
	string *name;
	string *auth_usr;
	string *auth_pwd;
	int fd;
	struct web_conf *next;
}web_conf_t;



typedef struct http_conf {
	int web_count;
	int port;
	int epfd;
	int fd;
	pool_t *p;
	string *auth_usr;
	string *auth_pwd;
	rkey_t *mimetype;
	web_conf_t *web;
}http_conf_t;



typedef struct response{
	size_t status_code;
	size_t content_length;

	buffer * server;
	buffer * date;
	buffer * *www_authenticate;
	buffer * content_type;
	buffer * physical_path;
	COMPRESS_TYPE content_encoding;	
}response;

typedef struct request{
	string * uri;
	string * host;
	string *args;

	
	string * authorization;
	string * user;
	string * pwd;
	http_method_t http_method;
	string * http_version;
	COMPRESS_TYPE accept_encoding;	
	string * content_length;
	buffer *header;

}request;


typedef struct http_connect{
	struct request *in;
	struct response *out;
	struct web_conf *web;
	struct pool_t *p;
	int fd;
	int (*next_handle)(http_conf_t *g, struct  http_connect *con);
}http_connect_t;


typedef struct epoll_extra_data {
	int type;
	void *ptr;
}epoll_extra_data_t;

typedef struct cgi_ev {
	char *ev[200];
	int count;
	int stdin;
	int stdout;

}cgi_ev_t;

typedef struct epoll_cgi {
	http_connect_t *con;
	int fd;
	buffer *cgi_data;
	buffer *out;
	
	
}epoll_cgi_t;


#define _Server "Rhttp" 
#define _Auth_desc "Rhttp test" 
#define _Version "0.3.0" 
#define DS_LINEEND "\x0a"


#endif
