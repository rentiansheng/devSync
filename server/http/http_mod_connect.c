#include "http_mod_connect.h"
#include <errno.h>
#include <stdio.h>

int
socket_listen(char *ip, unsigned short int port) 
{
	int res_socket;
	int res, on;
	struct sockaddr_in address;
	//struct in_addr in_ip;
	
	on = 1;
	res = res_socket = socket(AF_INET, SOCK_STREAM, 0);
	res = setsockopt(res_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	
	res = bind(res_socket, (struct sockaddr *)&address, sizeof(address));
	if(res) {printf("port is used, not to repear bind %s\n", strerror(errno));}
	res = listen(res_socket, MAX_CONNECT);

	return res_socket;
}

static int
make_fd_non_blocking(int sfd) 
{
	int flags, s;

	flags = fcntl(sfd, F_GETFL, 0);
	if(flags == -1) {
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if(s == -1) {
		return -1;
	}

	return 0;
}

int
epoll_init(long max) 
{
	return epoll_create(max);
}

static struct epoll_event *
epoll_init_r(int fd)
{
	struct epoll_event *ev;
	
	make_fd_non_blocking(fd);

	ev = (struct epoll_event *)malloc(sizeof(struct epoll_event));
	ev->data.fd = fd;
	ev->events = EPOLLIN|EPOLLET;

	return ev;
}

static struct epoll_event *
epoll_init_w(int fd)
{
	struct epoll_event *ev;
	
	make_fd_non_blocking(fd);

	ev = (struct epoll_event *)malloc(sizeof(struct epoll_event));
	ev->data.fd = fd;
	ev->events = EPOLLOUT|EPOLLET;

	return ev;
}


static struct epoll_event *
epoll_init_wr(int fd, int wr, void *extra)
{
	struct epoll_event *ev;
	
	make_fd_non_blocking(fd);

	ev = (struct epoll_event *)malloc(sizeof(struct epoll_event));
	if(extra == NULL) {
		ev->data.fd = fd;
	}
	else {
		ev->data.ptr = extra;
	}
	ev->events = (wr & EPOLL_W?EPOLLOUT:0)|(wr & EPOLL_R?EPOLLIN:0)|EPOLLET;
	
	return ev;
}
int 
epoll_add_fd(int epfd, int fd, int wr, void *extra)
{
	struct epoll_event *ev;

	ev = (struct epoll_event *)epoll_init_wr(fd, wr, extra);

	return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, ev);
}

int 
epoll_edit_fd(int epfd, struct epoll_event *ev, int wr)
{

	ev->events = (wr & EPOLL_W?EPOLLOUT:0)|(wr & EPOLL_R?EPOLLIN:0)|EPOLLET;
	return epoll_ctl(epfd, EPOLL_CTL_MOD, ev->data.fd, ev);
}

int
epoll_del_fd(int epfd, struct epoll_event *ev)
{
	return epoll_ctl(epfd, EPOLL_CTL_DEL, ev->data.fd, ev);
}

