/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#include "http_mod_connect.h"


int socket_listen_test(char *ip, unsigned short int port)  {
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
	if(res) {printf("port is used, not to repear bind %s\n", strerror(errno)); return -1;}

	return 0;
}

int socket_listen(char *ip, unsigned short int port)  {
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
	if(res) {printf("port is used, not to repear bind %s\n", strerror(errno)); exit(0);}
	res = listen(res_socket, MAX_CONNECT);

	return res_socket;
}



