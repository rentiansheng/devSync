/*
 * Copyright (C) Reage
 * blog:http://www.ireage.ccom
 */
#ifndef _HTTP_DS_DAEMON_H_

#define _HTTP_DS_DAEMON_H_
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include <pwd.h>

#include "base.h"

int ds_daemon(http_conf * conf);

#endif
