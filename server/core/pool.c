/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */

#include "pool.h"

static void * alloc_memalign(size_t alignment, size_t size);
static void * palloc_large(pool_t *pool, size_t size);
static void * palloc_pool(pool_t *pool, size_t size);

pool_t  * 
pool_create()
{
	pool_t *new;
	
	new = (pool_t  *)alloc_memalign(POOL_ALIGNMENT, POOL_DEFAULT_SIZE);
	if(new == NULL) {
		return NULL;
	}
	
	new->size = POOL_DEFAULT_SIZE - 1;
	new->current = &new->d;
	new->large = NULL;
	new->tmp = NULL;
	new->d.location = (uchar *) new + sizeof(pool_t);
	new->d.end = (uchar *)new + new->size;
	new->d.failed = 0;
	new->d.next = NULL;
	
	return new;
}

void
free_large_pool(pool_t *p)
{
	pool_large_t *l;
	
	l = p->large;

	while(l) {
		if(l->data) {
			free(l->data);
			l->data = NULL;
		}

		l = l->next;
	}
}

void 
pool_destroy(pool_t * p) {
	pool_data_t *tmp, *current;

	free_large_pool(p);

	current = p->d.next;
	free(p);

	while(current) {
		tmp = current;
		current = current->next;
		free(tmp);
	}
	
}

static void * 
palloc_pool(pool_t *pool, size_t size)
{
	pool_data_t *new, *tmp;
	uchar *m;

	m = (uchar *)alloc_memalign(POOL_ALIGNMENT, POOL_DEFAULT_SIZE);

	
	new = (pool_data_t *)m;
	m = (uchar *)m + sizeof(pool_data_t);
	new->location = m + size;
	new->end = (uchar *)m + pool->size;
	new->failed = 0;
	new->next = NULL;

	tmp = pool->current;
	while(tmp->next) {
		tmp = tmp->next;
	}

	tmp->next = new;
	
	return (void *) m;
}

static void * 
alloc(size_t size)
{
	void *p;

	p = malloc(size);
	if(p == NULL){
		
	}
	
	return p;
}

static void *
rg_calloc(size_t size)
{
	void * p;
	
	p = alloc(size);
	if(p){
		 memset(p, 0, size);
	}
	
	return 0;
}

static void *
alloc_memalign(size_t alignment, size_t size)
{
	void *p;
	int err;

	err = posix_memalign(&p, alignment, size);
	
	if(err){
		p = NULL;
	}
	
	return p;
}

static void * 
palloc_large(pool_t *pool, size_t size)
{
	void *data;
	pool_large_t * large;
	int n;
	
	data = alloc(size);
	
	if(data == NULL){
		return NULL;
	}
	
	n = 0;	
	
	for(large = pool->large; large; large = large->next, n++){
		if(large->data == NULL){
			large->data = data ;
			return data;
		}
		
		if(n > 2) {
			break;
		}
	}
	
	large = (pool_large_t *)palloc(pool, sizeof(pool_large_t));
	if(large == NULL){
		free(data);
		return NULL;
	}

	large->data = data;
	large->next = pool->large;
	pool->large = large;

	return data;
}


void *
palloc(pool_t *p, size_t size) {
	pool_data_t *current;
	uchar *m;
	
	if(size < p->size) {
		current = p->current;
		while(current) {
			m = current->location;
			if((size_t)(current->end - m) < size) {
				current->failed ++;
				current = current->next;
				continue;
			}
			
			current->location = m + size;

			return (void *)m;
		}

		return palloc_pool(p, size);

	}

	return palloc_large(p, size);
}

void *
pcalloc(pool_t *p, size_t size) {
	void * m;

	m = palloc(p, size);
	memset(m, 0, size);

	return m;
}
