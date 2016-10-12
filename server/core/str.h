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
void string_copy_str_n(const string *b, char *s1, int n);


#endif