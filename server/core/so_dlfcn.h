/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */
#ifndef _CORE_SO_DLFCN_H_
#define _CORE_SO_DLFCN_H_
#include "dlfcn.h"

typedef struct {
	char *path[255];
	char function[40];
	void *dp;
	void *f;
}so_t;

void get_function(so_t so);

void close_so(so_t so);



#endif
