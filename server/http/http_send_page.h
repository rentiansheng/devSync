/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */

#ifndef _HTTP_SEND_PAGE_H_
#define _HTTP_SEND_PAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/sendfile.h>
#include "base.h"
#include "http_request.h"
#include <errno.h>

extern int errno ;



int send_put_result(http_connect_t * con);

int send_put_forbidden_result(http_connect_t * con);

int send_execute(http_connect_t * con);

int send_put_header_err(http_connect_t *con);

#endif
