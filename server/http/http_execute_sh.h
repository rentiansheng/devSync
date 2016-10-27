#ifndef _HTTP_EXECUTE_SH_
#define _HTTP_EXECUTE_SH_
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



int  send_execute_sh_cmd(http_connect_t * con, http_conf * g );

#endif