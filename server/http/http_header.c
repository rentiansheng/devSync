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
		if(start >= end) return NULL;
		start ++;
	}

	return start;
}

static int parse_http_request_row(pool_t *p, request *in , char *start, char * end) {

	if(strncasecmp(start,"put", 3) == 0) {
		in->http_method = _PUT;
		start += 3;
	}else if(strncasecmp(start,"get", 3) == 0) {
		in->http_method = _GET;
		start += 3;
	}
	else if(strncasecmp(start, "post", 4) == 0) {
		in->http_method = _POST;
		start += 4;
	}else if(strncasecmp(start, "service", 7) == 0) {
		in->http_method = _SERVICE;
		start += 7;
	}else {
		in->http_method = _NONE;
	}

	start = skip_space(start, end);
	in->uri = string_init(p);
	in->uri->ptr = start;
	start = find_line(start, end);
	char *ptr = in->uri->ptr ;
	while(ptr < start && *ptr != ' ' && *ptr != '?') {
		ptr++;
	}

	in->uri->len = ptr - in->uri->ptr;

	return 0;
}

static int parse_content_length_compatible_old(request *in, char *start) {
	if(start == NULL) {in->content_length= 0; return 0; }
	in->content_length = atoi(start);

	return 0;
}

static int parse_http_header_messge(pool_t *p, request *in, char *start, char *end) {

	string *line = string_init(p);
	string *key = string_init(p);
	string *value = string_init(p);


	while(start != NULL && start < end) {
		if(*start == '\n') {
			start ++;
			continue;
		}
		string_get_line(start, end, line);
		if(line->len == 0 || line->ptr == NULL) {
			break;
		}
		string_get_word_with_split(line, key, ':');
		if(key->len == 0 || key->ptr == NULL){
			start = line->ptr + line->len;
			continue;
		}
		start += key->len+1;
		string_get_line(start , end, value);
		if(value->len == 0 || key->ptr == NULL) {
			start = line->ptr + line->len;
			continue;
		}
		if(strncasecmp("content-length", key->ptr, key->len) == 0) {
			in->content_length = atoi(key->ptr);
		}else if(strncasecmp("execute-file", key->ptr, key->len) == 0) {
			in->execute_file = string_init(p);
			in->execute_file->ptr = value->ptr;
			in->execute_file->len = value->len;
		}

		start = line->ptr + line->len;
	}

	return 0;

}




static void test_print_header(request *in)
{
	printf("uri=%s  len =%d\n", in->uri->ptr, in->uri->len);
	
	printf("content_length %d \n", in->content_length);

	if(in->execute_file != NULL) {
		printf("execute file %s %d", in->execute_file->ptr, in->execute_file->len);
	}


	printf("%s", in->header->ptr);

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
	
	parse_http_request_row(p, in, start, end); //请求行

	start = find_line(start, end);
	if(start != NULL) {
		start ++;
	}
	parse_content_length_compatible_old(in, start);

	parse_http_header_messge(p, in, start, end);



	//test_print_header(in);
	return ;
}





