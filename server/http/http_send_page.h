/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */

#ifndef _HTTP_SEND_PAGE_H_
#define _HTTP_SEND_PAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "http_request.h"


int send_put_result(http_connect_t * con);

int send_put_forbidden_result(http_connect_t * con);

#endif
