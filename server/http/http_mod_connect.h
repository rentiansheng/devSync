/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#ifndef _HTTP_MOD_CONNECT_H_
#define _HTTP_MOD_CONNECT_H_
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>




#define MAX_CONNECT 10000


int socket_listen_test(char *ip, unsigned short int port);

int socket_listen(char *ip, unsigned short int port);




#endif

