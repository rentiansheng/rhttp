/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */

#ifndef _CORE_BUFFER_H_
#define _CORE_BUFFER_H_
#include <stdio.h>
#include <stdlib.h>
#include "pool.h"

#define BUFFER_PIECE_SIZE 64/*Allocate memory base*/
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

typedef struct {
	char *ptr;
	size_t size;
} read_buffer;


typedef struct list_buffer {
	buffer *b;
	struct list_buffer *next;
}list_buffer;


read_buffer * read_buffer_init(pool_t *p);

read_buffer * read_buffer_init_str(pool_t *p, char *b, size_t len);

void read_buffer_get_line_split(read_buffer *src, read_buffer *dst, char split);

void read_buffer_get_line(read_buffer *src, read_buffer  *dst);

int read_buffer_compare(const read_buffer *s1, const read_buffer *s2);

int read_buffer_compare_str(const read_buffer *s1, const char *s2);

void read_buffer_to_str_n(read_buffer *b, char *s1, int n);

buffer * buffer_init(pool_t *p);

buffer * buffer_create_size(pool_t *p, size_t len);

int buffer_get_word_with_split(buffer *src, read_buffer *dst, char split);

int  buffer_get_line(buffer *src, read_buffer *dst);

void buffer_clear(buffer *b);

list_buffer * list_buffer_create(pool_t *p);

list_buffer * list_buffer_add(pool_t *p, list_buffer *lb);

list_buffer * list_buffer_last(list_buffer *lb);

void list_buffer_to_lower(list_buffer *lb);

void list_buffer_used_to_zero(list_buffer *lb);

int buffer_prepare_int(pool_t *p, buffer *b, size_t size);

int buffer_to_lower(buffer *b);

int buffer_to_upper(buffer *b);

int buffer_is_equal(buffer *a, buffer *b);

int buffer_caseless_cmpare(buffer *a, buffer *b);

int buffer_is_equal_len(buffer *a, buffer *b, size_t len);

int buffer_caseless_compare_len(buffer *a, buffer *b);

int buffer_path_simplify(buffer *dest, const buffer *src);



#endif
