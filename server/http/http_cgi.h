/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */
#ifndef _HTTP_CGI_H_
#define _HTTP_CGI_H_

#include "base.h"
#include "errno.h"
#include "str.h"

int add_cgi_handle(string *exce_file, exce_cgi_info_manager_t *cgi_info_manager);

int get_cgi_operator_handle(epoll_cgi_t *cgi_info, exce_cgi_info_manager_t *cgi_info_manager);

exce_cgi_info_manager_t * initCgiManager();
#endif
