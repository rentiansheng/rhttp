 #ifndef _HTTP_HEADER_H_
 #define _HTTP_HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "buffer.h"
#include "str.h"
#include "base.h"
#include "http_mod_connect.h"
#include "base64.h"
#include "http_send_page.h"

static int read_fd(list_buffer *header, pool_t *p, int fd);

static int read_http_header(list_buffer *header, pool_t *p, int fd);

int read_header(http_connect_t *con);

int read_cgi(epoll_cgi_t *cgi);

static int find_header_end(buffer *b);

static void adjust_header(list_buffer *pre, list_buffer *last);

static char * skip_space(char *start, char *end);

static char *find_end(char *start, char *end) ;

static void parse_http_uri(struct pool_t *p, request *in, string *rb);

void parse_header(http_connect_t * con);



static void test_print_header(request *in);

 #endif