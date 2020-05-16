/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#ifndef _HTTP_MOD_AUTHORIZED_H_
#define _HTTP_MOD_AUTHORIZED_H_
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "buffer.h"
#include "str.h"
#include "base.h"
#include "http_mod_connect.h"
#include "base64.h"
#include "http_file_write.h"
#include "http_delete.h"
#include "http_send_page.h"
#include "http_mod_authorized.h"
#include "modules/ds_log.h"

int http_mod_auth(http_connect_t *con);

#endif
