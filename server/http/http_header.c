/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */


#include "http_header.h"



static int read_http_header(buffer *header, pool_t *p, int fd){
	int palloc_size = 1024;
	char *c ;
	char *ptr;

	c = palloc(p,sizeof(char));
	header->ptr = palloc(p, palloc_size);
	header->size = palloc_size;


	while(read(fd, c, 1)) {
		buffer_append_char(header,*c,p);
		if(*c == '\n' && header->used >= 2) {
			ptr =  header->ptr + header->used - 2;
			if(strncasecmp(ptr, "\n\n", 2) == 0) {
				return 0;
			}
		}
	}

	return 0;
}

/**
 *0 读取结束，其他没有结束
 *
 */
int read_header(http_connect_t *con) {
	pool_t *p;
	buffer *header;


	p =(pool_t *) con->p;

	if(con->in->header == NULL)con->in->header = buffer_init(p);
	header = con->in->header;
	
	return read_http_header(header, p, con->fd);
}


static char * skip_space(char *start, char *end){
	if(start == NULL) return NULL;
	while( *start == ' ' ) {
		if(start >= end) return NULL;
		start ++;
	}

	return start;
}

static char * find_line(char *start, char *end) {
	while(*start != '\n') {
		if(start >= end) return end;
		start ++;
	}

	return start;
}

void parse_header(http_connect_t * con) {
 	request *in;
	buffer *header;
	pool_t *p;
	char * start, *end;


	p = (pool_t *)con->p;
	in = con->in;
	header = in->header;
	start = (char *)(header->ptr);
	end = (char *)header->ptr + (header->used - 1);
	if(strncasecmp(start,"put", 3) == 0) {
		in->http_method = _PUT;
		start += 3;
	}if(strncasecmp(start,"get", 3) == 0) {
		in->http_method = _GET;
		start += 3;
	}
	else if(strncasecmp(start, "post", 4) == 0) {
		in->http_method = _POST;
		start += 4;
	}

	start = skip_space(start, end);
	in->uri = string_init(p);
	in->uri->ptr = start;
	start = find_line(start, end);
	in->uri->len = start - in->uri->ptr;
	start++;


	in->content_length = atoi(start);
	//test_print_header(in);
	return ;
}
