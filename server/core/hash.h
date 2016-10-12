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
#include "memeory.h"

//必须是2的幂
#define HASH_DEFAULT_SIZE 8
#define HASH_ITEM_VALUE_TYPE_INT 1
#define HASH_ITEM_VALUE_TYPE_LONG 2
#define HASH_ITEM_VALUE_TYPE_DOUBLE 3
#define HASH_ITEM_VALUE_TYPE_STRING 4




union item_value {
    int i;
    long int l;
    double d;
    string * str;
};


typedef struct item {
    string * key;
    int type;
    item_value value;
    item * next;

}item;

typedef struct bucket{
    size_t count;
    item * items;
}bucket;

typedef struct hash{
    size_t count;
    size_t size;
    size_t mask;
    bucket *buckets;
}hash;

hash hash_init(pool_t *p);

static unsigned int hash_index(char *ptr, int len);

#endif