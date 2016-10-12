
/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */

#ifndef _HTTP_FILE_WRITE_H_INCLUDE
#define _HTTP_FILE_WRITE_H_INCLUDE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "base.h"
#include "http_request.h"
#include "http_send_page.h"
#include "http_mod_connect.h"
#include "modules/ds_log.h"



int  _mkdir(http_connect_t *con, char *root, pool_t *p);


int  open_write_file(http_connect_t *con);
int  write_file_content(http_connect_t *con);


#endif
