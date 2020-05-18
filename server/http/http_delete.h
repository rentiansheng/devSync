#ifndef _HTTP_DELETE_
#define _HTTP_DELETE_
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

int delete_resource(http_connect_t *con);

#endif