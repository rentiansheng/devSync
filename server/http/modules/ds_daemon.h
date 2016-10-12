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
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <pwd.h>
#include "http_mod_connect.h"

#include "base.h"

int ds_daemon(http_conf * conf, int t);

#endif
