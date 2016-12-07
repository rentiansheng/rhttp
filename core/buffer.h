/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */

#ifndef _CORE_BUFFER_H_
#define _CORE_BUFFER_H_
#include <stdio.h>
#include <stdlib.h>
#include "pool.h"
#include "str.h"

#define BUFFER_PIECE_SIZE 8/*Allocate memory base*/
#define BUFFER_ARRAY_SIZE 16/**/

typedef struct {
	char *ptr;

	size_t used;
	size_t size;
} buffer;

typedef struct {
	buffer **ptr;

	size_t used;
	size_t size;
} buffer_array;


typedef struct list_buffer {
	buffer *b;
	struct list_buffer *next;
}list_buffer;


buffer * buffer_init(pool_t *p);

buffer * buffer_create_size(pool_t *p, size_t len);

void buffer_append_char(pool_t *p, buffer *b, char c);

void buffer_append_str(pool_t *p, buffer *b, char *str, int len);

void buffer_append_n_str(pool_t *p, buffer *b, char *str, int len);

int buffer_get_word_with_split(buffer *src, string *dst, char split);

int  buffer_get_line(buffer *src, string *dst);

int buffer_add_prefix(pool_t *p, buffer *b, char *ptr, size_t len);


void buffer_clear(buffer *b);

list_buffer * list_buffer_create(pool_t *p);

list_buffer * list_buffer_add(pool_t *p, list_buffer *lb);

list_buffer * list_buffer_last(list_buffer *lb);

void list_buffer_to_lower(list_buffer *lb);


void list_buffer_used_to_zero(list_buffer *lb);

void buffer_find_str(buffer *src, buffer *dst, char *str);

void buffer_n_to_lower(buffer *src, int n);

int buffer_append_prepare(pool_t *p, buffer *b, size_t size);

int buffer_to_lower(buffer *b);

int buffer_to_upper(buffer *b);

int buffer_is_equal(buffer *a, buffer *b);

int buffer_caseless_cmpare(buffer *a, buffer *b);

int buffer_is_equal_len(buffer *a, buffer *b, size_t len);

int buffer_caseless_compare_len(buffer *a, buffer *b);

int buffer_path_simplify(buffer *dest, const buffer *src);

int buffer_append_context(pool_t *p, buffer *dst, const char *b, size_t len);

int buffer_add_str_end(pool_t *p, buffer *b) ;


static int buffer_prepare(pool_t *p, buffer * b, size_t size);


#endif
