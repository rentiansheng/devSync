
/*
 * Copyright (C) Reage
 * BLOG: http://www.rhttp.cn
 */


#ifndef _HTTP_REQUEST_H_INCLUDED_
#define _HTTP_REQUEST_H_INCLUDED_
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "buffer.h"
#include "base.h"
#include "http_mod_connect.h"
#include "http_virtual_port.h"
#include "base64.h"
#include "http_mod_authorized.h"
#include "http_send_page.h"
#include "http_send_html.h"
#include "http_autoindex.h"

#define MAX_CONNECT 10000
#define MAX_EVENT 800


#define HTTP_OK 200

#define HTTP_BAD_REQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_NOT_FOUND 404
#define HTTP_REQUEST_TIMEOUT 408
#define HTTP_NOT_IMPLEMENTED 501
#define HTTP_BAD_GATEWAY 502
#define HTTP_UNDEFINED 544


typedef enum{
	_GET,
	_POST,
	_NONE
}http_method_t;

typedef enum {
	_NOCOMPRESS, 
	_GZIP,
	_DEFLATE
}COMPRESS_TYPE;

typedef struct response{
	size_t status_code;
	size_t content_length;

	buffer * server;
	buffer * date;
	buffer * *www_authenticate;
	buffer * content_type;
	buffer * physical_path;
	read_buffer *args;
	COMPRESS_TYPE content_encoding;	
}response;

typedef struct request{
	read_buffer * uri;
	read_buffer * host;
	
	read_buffer * authorization;
	read_buffer * user;
	read_buffer * pwd;
	http_method_t http_method;
	read_buffer * http_version;
	COMPRESS_TYPE accept_encoding;	
	
	struct list_buffer *header;

}request;



int start_accept(struct http_conf *g);

response * response_init(pool_t *p);
request * request_init(pool_t *p);

#endif
