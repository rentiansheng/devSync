
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
#include "base64.h"
#include "http_file_write.h"
#include "http_send_page.h"


int start_accept();

response * response_init(pool_t *p);
request * request_init(pool_t *p);

#endif
