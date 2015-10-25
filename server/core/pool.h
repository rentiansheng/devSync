/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */

#ifndef _CORE_POOL_H_
#define _CORE_POOL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  POOL_DEFAULT_SIZE (4096)
#define  POOL_ALIGNMENT 16
#define uchar unsigned char


typedef struct pool_data_t{
	uchar *location;
	uchar *end;
	size_t failed;
	struct pool_data_t * next;
}pool_data_t;

typedef struct pool_large_t{
	struct pool_large_t * next;
	uchar *data;
}pool_large_t;

typedef struct pool_t{
	pool_data_t d;
	size_t size;
	pool_data_t *current;
	pool_large_t *large;
	pool_data_t *tmp;
	
}pool_t;


pool_t * pool_create();
void pool_destroy(pool_t * p);


void * palloc(pool_t *p, size_t size);
//void * pnalloc(pool_t *p, size_t size);
void * pcalloc(pool_t *p, size_t size);
//void * pncalloc(pool_t *p, size_t size);

#endif
