/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#ifndef _HTTP_BASE_H_
#define _HTTP_BASE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include "buffer.h"
#include "str.h"
#include "pool.h"
#include "http_mod_connect.h"

#define OK 0
#define FILE_NO_EXIST 1
#define FILE_NO_ACCESS 2
#define CANCEL 3
#define DONE 4
#define MEMERROR -1
#define UNDEFINED   -144

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
#define HTTP_WRITE_FILE_FAIL 406

#define HANDLE_STATUS_OK 0
#define HANDLE_STATUS_CONTINUE 1
#define HANDLE_STATUS_FAILD -1



typedef enum{
	_GET,
	_POST,
	_PUT,
	_NONE
}http_method_t;

typedef enum {
	_NOCOMPRESS, 
	_GZIP,
	_DEFLATE
}COMPRESS_TYPE;



typedef enum {
	READ_HEAD,
	HEAD_PRASE,
	HANDLE,
	COMPRESS,
	WRITE,
}step;

typedef enum {
	t_char,
	t_uchar,
	t_int,
	t_uint,
	t_long,
}value_type;

typedef struct key {
	char *name;
	char *value;
	value_type type;
	struct key *next;
}key;


typedef struct fileinfo {
	string * name;
	size_t len;
	FILE * fp;
	void *start;
}fileinfo_t;

typedef struct mananger_con {
	struct mananger_con *left;
	struct mananger_con *right;
	void * val;
	struct epoll_event *ev;
	int type ;
	int time;
}mananger_con;


typedef struct web_conf {
	char *root;
	char *index_file;
	int index_count;
	char *err404;
	char *server;
	int fd;
	struct web_conf *next;
}web_conf;

typedef struct http_conf {
	int web_count;
	int port;
	int epfd;
	int fd;
	char *user;
	key *mimetype;
	web_conf *web;
}http_conf;



typedef struct response{
	size_t status_code;
	size_t content_length;

	buffer * server;
	buffer * date;
	buffer * *www_authenticate;
	buffer * content_type;
	buffer * physical_path;
	COMPRESS_TYPE content_encoding;	
}response;

typedef struct request{
	string * uri;
	string * host;
	string *args;
	string *clientIp;
	
	string * authorization;
	string * user;
	string * pwd;
	http_method_t http_method;
	string * http_version;
	COMPRESS_TYPE accept_encoding;
	unsigned int  content_length;
	buffer *header;

}request;


typedef struct http_connect{
	request *in;
	response *out;
	fileinfo_t write_file;
	web_conf *web;
	pool_t *p;
	int fd;
	int (*next_handle)(struct  http_connect *con);
}http_connect_t;


typedef struct epoll_extra_data {
	int type;
	void *ptr;
}epoll_extra_data_t;

typedef struct cgi_ev {
	char *ev[20];
	int count;
	int stdin;
	int stdout;
	

}cgi_ev_t;

typedef struct epoll_cgi {
	http_connect_t *con;
	int fd;
	struct list_buffer *cgi_data;
	list_buffer *out;
}epoll_cgi_t;


#define _Server "DevSync"
#define _Auth_desc "DevSync"
#define _Version "0.2.1"
#define DS_LINEEND "\x0a"

#endif
