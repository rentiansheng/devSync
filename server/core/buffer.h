/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */

#ifndef _CORE_BUFFER_H_
#define _CORE_BUFFER_H_
#include <stdio.h>
#include <stdlib.h>
#include "pool.h"
#include "str.h"


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


typedef struct list_buffer_item {
	char *ptr;
	size_t used;
	struct list_buffer_item * next;
}list_buffer_item_t;

typedef struct list_buffer {
	list_buffer_item_t *head;
	list_buffer_item_t *foot;
	size_t size;
}list_buffer_t;




buffer * buffer_init(pool_t *p);

buffer * buffer_create_size(pool_t *p, size_t len);

void buffer_append_char(buffer *b, char c, pool_t *p);

int buffer_get_word_with_split(buffer *src, struct string *dst, char split);

int  buffer_get_line(buffer *src, struct string *dst);

void buffer_clear(buffer *b);

list_buffer_t * list_buffer_create(pool_t *p, size_t size );

int  list_buffer_add(pool_t *p, list_buffer_t *b);

list_buffer_t * list_buffer_last(list_buffer_t *lb);

void buffer_find_str(buffer *src, buffer *dst, char *str);

void buffer_n_to_lower(buffer *src, int n);

int buffer_prepare_int(pool_t *p, buffer *b, size_t size);

int buffer_to_lower(buffer *b);

int buffer_to_upper(buffer *b);

int buffer_is_equal(buffer *a, buffer *b);

int buffer_caseless_cmpare(buffer *a, buffer *b);

int buffer_is_equal_len(buffer *a, buffer *b, size_t len);

int buffer_caseless_compare_len(buffer *a, buffer *b);

int buffer_path_simplify(buffer *dest, const buffer *src);

void buffer_append_str(buffer *b, char *str, int len, pool_t *p);

void buffer_append_long(buffer *b, long value, pool_t *p) ;

void buffer_append_n_str(buffer *b, char *str, int len, pool_t *p);


#endif
