/*
 * Copyright (C) Reage
 * blog:http://www.rhttp.cn
 */
#ifndef _CGI_HANDLE_H_
#define _CGI_HANDLE_H_
#include <signal.h>
#include "base.h"
#include "errno.h"
#include "str.h"
#include "linux_epoll.h"
#include "ds_log.h"

extern char **environ;

void cgi_handle(epoll_cgi_t *cgi_info, http_conf *g);

int init_cgi_data_struct(string *execute_file, execute_cgi_info_manager_t *cgi_info_manager, unsigned int ts);

int get_cgi_operator_handle(epoll_cgi_t *cgi_info);

int cgi_del(http_connect_t *con);

#endif
