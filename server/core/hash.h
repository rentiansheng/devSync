/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#ifndef _CORE_HASH_H_
#define _CORE_HASH_H_
#include <stdio.h>
#include <stdlib.h>
#include "pool.h"
#include "str.h"
#include <string.h>

//必须是2的幂
#define HASH_DEFAULT_SIZE 8
#define HASH_ITEM_VALUE_TYPE_INT 1
#define HASH_ITEM_VALUE_TYPE_LONG 2
#define HASH_ITEM_VALUE_TYPE_DOUBLE 3
#define HASH_ITEM_VALUE_TYPE_STRING 4
#define HASH_ITEM_VALUE_TYPE_PTR 5



typedef union hash_item_value {
    int i;
    long int l;
    double d;
    string * str;
    void * ptr;
}hash_item_value_u;


typedef struct hash_item {
    string * key;
    int type;
    hash_item_value_u value;
    struct hash_item * next;

}hash_item_t;

typedef struct bucket{
    size_t count;
    hash_item_t * items;
}bucket_t;

typedef struct hash{
    size_t count;
    size_t size;
    size_t mask;
    bucket_t *buckets;
}hash_t;

hash_t * hash_init(pool_t *p);
int hash_add_int(pool_t *p, hash_t *h, const char *key, int keyLen, int  v) ;
int hash_add_long(pool_t *p,hash_t *h, const char *key, int keyLen, long  v);
int hash_add_double(pool_t *p, hash_t *h,const char *key, int keyLen, double v);
int hash_add_string(pool_t *p,hash_t *h, const char *key, int keyLen, string *v);
int hash_add_ptr(pool_t *p, hash_t *h, const char *key, int keyLen, void * v);
int hash_get_int(hash_t *h, const char *key, int keyLen, int * ret);
int hash_get_long(hash_t *h, const char *key, int keyLen, long int * ret);
int hash_get_double(hash_t *h, const char *key, int keyLen, double * ret);
int hash_get_string(hash_t *h, const char *key, int keyLen, string * ret);
int hash_get_ptr(hash_t *h, const char *key, int keyLen, void * ret, size_t size);
int hash_del_item(hash_t *h, const char *key, int keyLen);
int hash_add_item(hash_t *h, hash_item_t *newItem);
static int hash_add(pool_t *p, hash_t *h ,const char * key, int keyLen, void * ptr, int type);
hash_item_t * get_hash_item(hash_t *h, const char *key, int keyLen);
static unsigned int  hash_reinit(pool_t *p, hash_t *h);

static unsigned int hash_index(hash_t * h, char *ptr, int len);

#endif