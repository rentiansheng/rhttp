/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */

#ifndef _CORE_STR_H_
#define _CORE_STR_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pool.h"

typedef  struct string {
    char *ptr;
    int len;
}string;

string * string_init(pool_t *p);
string * string_init_from_str(pool_t *p, char *b, size_t len);
string * string_init_from_ptr(pool_t *p, char *ptr, size_t len);
int string_compare(const string *s1, const string *s2);
int string_compare_str(const string *s1, const char *s2);
void string_copy_n_to_str(const string *b, char *s1, int n);
int  string_get_line(char  *start, char *end, string *dst);
void string_get_word_with_split(string * src, string *dst, char split);
void string_get_line_split(string *src, string *dst, char split);
void string_skip_char_left(string *str, char c);
void string_split_kv(string * src, string *key, string *value, char split);

int raw_str_ncmp(char * s1, char * s2, int len);

void string_skip_char(string *str, char c) ;

#endif