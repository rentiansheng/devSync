/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */
#ifndef _HTTP_SERVER_CGI_H_
#define _HTTP_SERVER_CGI_H_

#include "base.h"
#include "http_request.h"
#include "http_mod_connect.h"
#include "errno.h"
#include "string.h"

int cgi_handle(http_conf *g, http_connect_t *con);


#endif
