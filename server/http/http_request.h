
/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */


#ifndef _HTTP_REQUEST_H_INCLUDED_
#define _HTTP_REQUEST_H_INCLUDED_
#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "str.h"
#include "base.h"
#include "http_mod_connect.h"
#include "base64.h"
#include "http_header.h"
#include "http_file_write.h"
#include "http_send_page.h"
#include "modules/ds_log.h"
#include "linux_epoll.h"




int start_accept();

response * response_init(pool_t *p);
request * request_init(pool_t *p);

#endif
