/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */
#ifndef _HTTP_CONFIG_H_
#define _HTTP_CONFIG_H_
#include "base.h"

char is_char(int c);

int config_init(char *path, http_conf_t *conf);


int parse_line(FILE *f, char *line,int  *row, http_conf_t *conf) ;


int set_mimetype(FILE * f, http_conf_t *conf, int *row);

int set_web(FILE *f, http_conf_t *conf, int *row);


int read_line(FILE *f, char *line, int *row);


#endif
