/*
 * Copyright (C) Reage
 * blog:http://www.ireage.ccom
 */
#ifndef _HTTP_DS_LOG_H_

#define _HTTP_DS_LOG_H_
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "base.h"

#define  LOG_STR_LEN (1024)
#define  LOG_LEVEL_DEFAULT 1
#define  LOG_LEVEL_ERROR 16



int logFileId;
int logFileErrorId;

int ds_log(http_connect_t * con, char * str, int level);
int ds_log_error(http_conf * conf, char * str);
int ds_init(http_conf * con);

#endif
