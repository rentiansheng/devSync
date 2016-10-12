/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */

 #ifndef _LINUX_EPOLL_H_
 #define _LINUX_EPOLL_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>



#define EPOLL_W 1
#define EPOLL_R 2
#define SERVERFD 1
#define SOCKFD 2
#define CGIFD 3
#define WRITEFILE 4


int epoll_init(long max); 


int  epoll_edit_fd(int epfd, struct epoll_event *ev, int wr);

int  epoll_add_fd(int epfd, int fd, int wr, void *extra);


int epoll_del_fd(int epfd, struct epoll_event *ev);

#endif