/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#include "http_send_page.h"

int send_put_result(http_connect_t * con)
{
	response *out = con->out;
	pool_t *p = con->p;
	char *msg =  (char *) palloc(p, sizeof(char)*1024);
	sprintf(msg, "HTTP/1.1 200  OK\r\nConnection: Close\r\nContent-Type: text/html\r\n");
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);

	if(out != NULL && out->status_code == HTTP_OK) {
		sprintf(msg, "%ssuccess\r\n\r\n", msg);
	} else {
		sprintf(msg, "%serror\r\n\r\n", msg);
	}

   write (con->fd, msg, strlen(msg));

   con->next_handle = NULL;
   return -1;
}

int send_put_forbidden_result(http_connect_t * con)
{
	response *out = con->out;
	pool_t *p = con->p;
	char *msg =  (char *) palloc(p, sizeof(char)*1024);
	sprintf(msg, "HTTP/1.1 403  Forbidden\r\nConnection: Close\r\nContent-Type: text/html\r\n");
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);

	sprintf(msg, "%s403 Forbidden\r\n\r\n", msg);


   write (con->fd, msg, strlen(msg));

   con->next_handle = NULL;
   return -1;
}



