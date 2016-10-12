/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */

 #ifndef _HTTP_HEADER_H
 #define _HTTP_HEADER_H 

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "buffer.h"
#include "str.h"
#include "base.h"
#include "http_mod_connect.h"
#include "base64.h"
#include "http_file_write.h"
#include "http_send_page.h"
#include "modules/ds_log.h"

static int read_http_header(buffer *header, pool_t *p, int fd);

static char * skip_space(char *start, char *end);

static char * find_line(char *start, char *end);

void parse_header(http_connect_t * con);

int read_header(http_connect_t *con);



 #endif